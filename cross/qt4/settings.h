#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtGui>

class Settings
{
public:
    QString interface;

    enum Position { TopLeft, TopRight, BottomLeft, BottomRight };

    Position position;

    enum Units { Bps, Kps, Mps, Gps };

    Units units;

    enum Prefix { Bits, Si, IEC };

    Prefix prefix;

    bool prefixDisplay;

    QFont fontFamily;

    QColor fontColor;

    Settings();

    ~Settings();

    void set(const QString&, const QVariant&);

private:
    QSettings settings;
};

#endif /* SETTINGS_H */
