#include <QStringList>
#include <QString>
#include <QFile>
#include <QDir>
#include <QIODevice>

#include "application.h"
#include "monitor.h"

#define INTERFACE_PATH "/sys/class/net"

#define INTERFACE_VALUE "/sys/class/net/%1/statistics/%2_bytes"

class Monitor::impl {
public:
    impl(const QString&);
    ~impl();

    Data poll();

    QString getInterface() const;

private:
    impl();
    impl(const impl&);
    impl& operator=(const impl&);

    QString name;

    quint64 rx;
    quint64 tx;

    bool initialized;

    quint64 readInt(const QString&);
};

Monitor::impl::impl(const QString& string) :
    name(string), rx(0), tx(0), initialized(false) {}

Monitor::impl::~impl() {}

quint64 Monitor::impl::readInt(const QString& xx) {
    QString path(QString(INTERFACE_VALUE).arg(name).arg(xx));

    QFile file(path);

    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream stream(&file);

    quint64 value;

    stream >> value;

    return value;
}

QString Monitor::impl::getInterface() const {
    return name;
}

Data Monitor::impl::poll() {
    quint64 rxnew = readInt("rx");

    quint64 txnew = readInt("tx");

    // Set initial poll value to 0.0.
    if (!initialized) {
        initialized = true;
        rx = rxnew;
        tx = txnew;
    }

    Data data = {rxnew - rx, txnew - tx};

    rx = rxnew;
    tx = txnew;

    return data;
}

Monitor::Monitor(const QString& string)
{
    setInterface(string);
}

Monitor::~Monitor() {}

Data Monitor::poll()
{
    return pimpl->poll();
}

void Monitor::setInterface(const QString& string)
{
    QStringList list = interfaces();

    if (list.contains(string)) {
        pimpl.reset(new impl(string));
    }
    else {
        // Then either string == "undefined" or the interface does not
        // exist. So just pick the first element in the list, assuming
        // that there is at least one device.
        pimpl.reset(new impl(list[0]));
    }
}

QString Monitor::getInterface() const {
    return pimpl->getInterface();
}

QStringList Monitor::interfaces() const
{
    QStringList list;

    QString path(INTERFACE_PATH);

    QDir dir(path);

    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

    dir.setSorting(QDir::Name);

    return dir.entryList();
}
