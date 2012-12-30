#include <windows.h>
#include <stdio.h>

#include <pdh.h>
#include <pdhmsg.h>

#include <stdint.h>

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
    impl(const wchar_t*);
    ~impl();
    PDH_STATUS poll(Data&);

private:
    impl();
    impl(const impl&);
    impl& operator=(const impl&);

    HCOUNTER recv_counter;
    HCOUNTER sent_counter;
    HQUERY handle;

    PDH_STATUS init(const wchar_t*);
};

Monitor::impl::impl(const wchar_t *interface_str)
{
    PDH_STATUS status;

    status = init(interface_str);
    if (status != ERROR_SUCCESS) {
        throw; // Why not!
    }
}

#define check(call)                                                     \
    if (status != ERROR_SUCCESS) {                                      \
        fprintf(stderr, call##" failed with status 0x%x\n", status);    \
        return status;                                                  \
    }

PDH_STATUS Monitor::impl::init(const wchar_t *interface_str)
{
    PDH_STATUS status;

    status = PdhOpenQuery(NULL, NULL, &handle);
    check("PdhOpenQuery");

    wchar_t buffer[BUFSIZ];

    wsprintf(buffer, L"\\Network Interface(%s)\\Bytes Received/sec", interface_str);

    status = PdhAddCounter(handle, buffer, 0, &recv_counter);
    check("PdhAddCounter 'recv'");

    wsprintf(buffer, L"\\Network Interface(%s)\\Bytes Sent/sec", interface_str);

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

    data.rx = static_cast<uint64_t>(value.longValue);

    status = PdhGetFormattedCounterValue(
        sent_counter,
        PDH_FMT_LONG,
        NULL,
        &value);

    check("PdhGetFormattedCounterValue 'send'");

    data.tx = static_cast<uint64_t>(value.longValue);

    return status;
}

Monitor::impl::~impl()
{
    if (handle) {
        PdhCloseQuery(handle);
    }
}

Monitor::Monitor(size_t interface_id)
{
    switch_interface(interface_id);
}

Monitor::~Monitor()
{
}

Data Monitor::poll()
{
    Data d;

    pimpl->poll(d);

    return d;
}

void Monitor::switch_interface(size_t interface_id)
{
    vector<wstring> interface_list = interfaces();

    if (interface_id >= interface_list.size())
        interface_id = 0;

    const wchar_t *str = L"*"; // Default "*" equates to "All Interfaces"

    if (interface_id > 0)
        str = interface_list[interface_id].c_str();

    pimpl.reset(new impl(str));
}

vector<wstring> Monitor::interfaces()
{
    CONST PWSTR COUNTER_OBJECT = L"Network Interface";
    PDH_STATUS status = ERROR_SUCCESS;
    LPWSTR counters = NULL;
    DWORD nbytes_counters = 0;
    LPWSTR instances = NULL;
    DWORD nbytes_instances = 0;
    LPWSTR temp = NULL;

    vector<wstring> v;

    v.push_back(L"All interfaces"); // Initial default value.

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
        v.push_back(temp);

FreeAll:
    free(instances);
FreeCounters:
    free(counters);
Error:
    return v;
}
