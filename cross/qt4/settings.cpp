#include <QtGui>

#include "settings.h"
#include "resource.h"

Settings::Settings() :
    settings(QSettings::IniFormat,
             QSettings::UserScope,
             MONITOR_ORGINIZATION_NAME,
             MONITOR_APPLICATION_NAME)
{
    interface = settings.value("interface", QString(MONITOR_DEFAULT_INTERFACE)).value<QString>();

    units = static_cast<Units>(settings.value("units", MONITOR_DEFAULT_UNITS).toInt());

    prefix = static_cast<Prefix>(settings.value("prefix", MONITOR_DEFAULT_PREFIX).toInt());

    prefixDisplay = settings.value("prefixDisplay", MONITOR_DEFAULT_PREFIX_DISPLAY).toBool();

    position = static_cast<Position>(settings.value("position", MONITOR_DEFAULT_POSITION).toInt());

    QFont tnof(MONITOR_DEFAULT_FONT_FAMILY, MONITOR_DEFAULT_FONT_SIZE);

    fontFamily = settings.value("fontFamily", tnof).value<QFont>();

    fontColor = settings.value("fontColor", MONITOR_DEFAULT_FONT_COLOR).value<QColor>();
}

Settings::~Settings() {
    settings.sync();
}

void Settings::set(const QString& key, const QVariant& value) {
#define QVARIANT(to) value.to()
#define ENUM(to) static_cast<to>(value.toInt())
#define CHECK(setting, type)           \
    if (key == #setting) {             \
        setting = type;                \
        settings.setValue(key, value); \
        settings.sync();               \
    }
    CHECK(interface, QVARIANT(toString))
    CHECK(units, ENUM(Units))
    CHECK(prefix, ENUM(Prefix))
    CHECK(position, ENUM(Position))
    CHECK(prefixDisplay, QVARIANT(toBool))
    CHECK(fontFamily, QVARIANT(value<QFont>))
    CHECK(fontColor, QVARIANT(value<QColor>))
#undef CHECK
#undef ENUM
#undef QVARIANT
}
