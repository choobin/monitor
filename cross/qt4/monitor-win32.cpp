#include <QStringList>
#include <QString>

#include <windows.h>
#include <stdio.h>

#include <pdh.h>
#include <pdhmsg.h>

#pragma comment(lib, "pdh.lib")

#include <vector>
#include <string>
#include <memory>
using std::vector;
using std::wstring;
using std::unique_ptr;

#include "monitor.h"

class Monitor::impl {
public:
    impl(const QString&);
    ~impl();

    PDH_STATUS poll(Data&);

    QString getInterface() const;

private:
    impl();
    impl(const impl&);
    impl& operator=(const impl&);

    QString name;

    HCOUNTER recv_counter;
    HCOUNTER sent_counter;
    HQUERY handle;

    PDH_STATUS init(const QString&);
};

Monitor::impl::impl(const QString& string) : name(string)
{
    PDH_STATUS status;

    status = init(string);
    if (status != ERROR_SUCCESS) {
        throw; // Why not!
    }
}

#define check(call)                                                     \
    if (status != ERROR_SUCCESS) {                                      \
        fprintf(stderr, call##" failed with status 0x%x\n", status);    \
        return status;                                                  \
    }

PDH_STATUS Monitor::impl::init(const QString& string)
{
    PDH_STATUS status;
    wchar_t buffer[BUFSIZ];
    int length;

    status = PdhOpenQuery(NULL, NULL, &handle);
    check("PdhOpenQuery");

    QString recv(QString("\\Network Interface(%1)\\Bytes Received/sec").arg(string));

    length = recv.toWCharArray(buffer);
    buffer[length] = '\0';

    status = PdhAddCounter(handle, buffer, 0, &recv_counter);
    check("PdhAddCounter 'recv'");

    QString sent(QString("\\Network Interface(%1)\\Bytes Sent/sec").arg(string));

    length = sent.toWCharArray(buffer);
    buffer[length] = '\0';

    status = PdhAddCounter(handle, buffer, 0, &sent_counter);
    check("PdhAddCounter 'sent'");

    status = PdhCollectQueryData(handle);
    check("PdhCollectQueryData");

    return status;
}

PDH_STATUS Monitor::impl::poll(Data& data)
{
    PDH_STATUS status;
    PDH_FMT_COUNTERVALUE value;

    status = PdhCollectQueryData(handle);
    check("PdhCollectQueryData");

    status = PdhGetFormattedCounterValue(
        recv_counter,
        PDH_FMT_LONG,
        NULL,
        &value);

    check("PdhGetFormattedCounterValue 'recv'");

    data.rx = static_cast<quint64>(value.longValue);

    status = PdhGetFormattedCounterValue(
        sent_counter,
        PDH_FMT_LONG,
        NULL,
        &value);

    check("PdhGetFormattedCounterValue 'sent'");

    data.tx = static_cast<quint64>(value.longValue);

    return status;
}

QString Monitor::impl::getInterface() const
{
    return name;
}

Monitor::impl::~impl()
{
    if (handle) {
        PdhCloseQuery(handle);
    }
}

Monitor::Monitor(const QString& string)
{
    setInterface(string);
}

Monitor::~Monitor()
{
}

Data Monitor::poll()
{
    PDH_STATUS status;
    Data d;

    status = pimpl->poll(d);
    if (status != ERROR_SUCCESS) {
        fprintf(stderr, "Monitor::impl 'poll' failed with status 0x%x\n", status);
    }

    return d;
}

void Monitor::setInterface(const QString& string)
{
    QStringList list = interfaces();

    QString value("*"); // Default 'All Interfaces'

    if (string != "All interfaces" &&
        list.contains(string)) {
        value = string;
    }

    pimpl.reset(new impl(value));
}

QString Monitor::getInterface() const
{
    QString string = pimpl->getInterface();

    if (string == "*")
        return "All interfaces";

    return string;
}

QStringList Monitor::interfaces() const
{
    CONST PWSTR COUNTER_OBJECT = L"Network Interface";
    PDH_STATUS status = ERROR_SUCCESS;
    LPWSTR counters = NULL;
    DWORD nbytes_counters = 0;
    LPWSTR instances = NULL;
    DWORD nbytes_instances = 0;
    LPWSTR temp = NULL;

    QStringList interfaces;

    interfaces << "All interfaces"; // Initial default value.

    // Determine the required buffer size for the data.
    status = PdhEnumObjectItems(
        NULL,
        NULL,
        COUNTER_OBJECT,
        counters,
        &nbytes_counters,
        instances,
        &nbytes_instances,
        PERF_DETAIL_WIZARD,
        0);

    if (status != PDH_MORE_DATA)
        goto Error;

    // Keep this here. I might want to do something with it later.
    counters = (LPWSTR)malloc(nbytes_counters * sizeof(wchar_t));
    if (counters == NULL)
        goto Error;

    instances = (LPWSTR)malloc(nbytes_instances * sizeof(wchar_t));
    if (instances == NULL)
        goto FreeCounters;

    // The second call will fill the counter and instance buffers.
    status = PdhEnumObjectItems(
        NULL,
        NULL,
        COUNTER_OBJECT,
        counters,
        &nbytes_counters,
        instances,
        &nbytes_instances,
        PERF_DETAIL_WIZARD,
        0);

    if (status != ERROR_SUCCESS)
        goto FreeAll;

    for (temp = instances; *temp != 0; temp += wcslen(temp) + 1)
        interfaces << QString::fromWCharArray(temp);

FreeAll:
    free(instances);
FreeCounters:
    free(counters);
Error:
    return interfaces;
}
