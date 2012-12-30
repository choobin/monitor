#ifndef MONITOR_H
#define MONITOR_H

#include <QString>
#include <QStringList>

#include <memory>

struct Data {
    quint64 rx;
    quint64 tx;
};

class Monitor {
public:
    Monitor(const QString&);

    ~Monitor();

    Data poll();

    QStringList interfaces() const;

    void setInterface(const QString&);

    QString getInterface() const;

private:
    Monitor();
    Monitor(const Monitor&);
    Monitor& operator=(const Monitor&);

    class impl;
    std::unique_ptr<impl> pimpl;
};

#endif /* MONITOR_H */
