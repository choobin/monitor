#include <QtGui>

#include "application.h"
#include "monitor.h"
#include "resource.h"
#include "settings.h"
#include "ui.h"

Ui::Ui(Settings *s, Monitor *m, QWidget *parent) :
    QWidget(parent),
    settings(s),
    monitor(m)
{
    setWindowTitle(MONITOR_APPLICATION_NAME);

    setAttribute(Qt::WA_TranslucentBackground);

    setWindowFlags(Qt::FramelessWindowHint | Qt::CustomizeWindowHint);

    setWindowIcon(QIcon(MONITOR_SVG));

    createWidgets();

    buildMenu();

    createTrayIcon();

    updateLabels();

    updatePrefix();

    show();

    startTimer(1000);
}

void Ui::selectInterface(int index) {
    foreach(QAction *action, interfaceActions) {
        boldify(action, false);
    } // :D

    boldify(interfaceActions[index], true);

    settings->set("interface", interfaceActions[index]->text());

    monitor->setInterface(interfaceActions[index]->text());
}

void Ui::selectUnits(int units) {
    boldify(unitsActions[settings->units], false);

    settings->set("units", units);

    boldify(unitsActions[settings->units], true);

    updatePrefix();
}

void Ui::selectPrefix(int prefix) {
    boldify(prefixActions[settings->prefix], false);

    settings->set("prefix", static_cast<Settings::Prefix>(prefix));

    boldify(prefixActions[settings->prefix], true);

    updatePrefix();
}

void Ui::selectPosition(int position) {
    boldify(positionActions[settings->position], false);

    settings->set("position", static_cast<Settings::Position>(position));

    boldify(positionActions[settings->position], true);

    updatePosition();
}

void Ui::selectPrefixDisplay() {
    bool toggle = (settings->prefixDisplay) ? false: true;

    boldify(prefixDisplayAction, toggle);

    settings->set("prefixDisplay", toggle);

    boldify(prefixDisplayAction, toggle);

    updatePrefix();
}

void Ui::selectFont() {
    bool ok;

    QFont font = QFontDialog::getFont(&ok, settings->fontFamily, NULL);

    if (ok) {
        settings->set("fontFamily", font);

        updateLabels();

        updatePosition();
    }
}

void Ui::selectColor() {
    QColor color = QColorDialog::getColor(settings->fontColor, NULL);

    if (color.isValid()) {
        settings->set("fontColor", color);

        updateLabels();
    }
}

void Ui::displayAbout() {
    QMessageBox about(this);

    about.setIconPixmap(QPixmap(MONITOR_SVG));

    about.setWindowTitle("About Monitor");

    about.setTextFormat(Qt::RichText);

    about.setText(
        MONITOR_APPLICATION_NAME " " MONITOR_VERSION_STR "<br/><br/>"
        MONITOR_DESCRIPTION "<br/><br/>"
        "<a href='" MONITOR_URL "'>" MONITOR_URL "</a><br/><br/>"
        MONITOR_AUTHOR);

    about.exec();

    updatePosition();
}

void Ui::createWidgets() {
    rxlabel = new QLabel();

    txlabel = new QLabel();

    rxprefix = new QLabel();

    txprefix = new QLabel();

    if (!settings->prefixDisplay) {
        rxprefix->setVisible(false);

        txprefix->setVisible(false);
    }

    QGridLayout *layout = new QGridLayout(this);

    layout->setVerticalSpacing(0);

    layout->setHorizontalSpacing(2);

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(rxlabel, 0, 0, Qt::AlignRight);

    layout->addWidget(rxprefix, 0, 1);

    layout->addWidget(txlabel, 1, 0, Qt::AlignRight);

    layout->addWidget(txprefix, 1, 1);

    setLayout(layout);
}

void Ui::boldify(QAction *action, bool boldification) {
    QFont font = action->font();

    font.setBold(boldification);

    action->setFont(font);

    action->setChecked(boldification);
}

// Ooooooooooooh.
#define buildSubmenu(name, lhscheck, rhscheck, actions, slot) do {      \
        QMenu *submenu = new QMenu(name);                               \
                                                                        \
        QSignalMapper *mapper = new QSignalMapper(this);                \
                                                                        \
        int index = 0;                                                  \
                                                                        \
        foreach(QString string, list) {                                 \
            QAction *action = new QAction(string, this);                \
                                                                        \
            action->setCheckable(true);                                 \
                                                                        \
            connect(action, SIGNAL(triggered()), mapper, SLOT(map()));  \
                                                                        \
            mapper->setMapping(action, index);                          \
                                                                        \
            submenu->addAction(action);                                 \
                                                                        \
            if (lhscheck == rhscheck)                                   \
                boldify(action);                                        \
                                                                        \
            actions.push_back(action);                                  \
                                                                        \
            index++;                                                    \
        }                                                               \
                                                                        \
        connect(mapper, SIGNAL(mapped(int)), this, SLOT(slot(int)));    \
                                                                        \
        qmenu->addMenu(submenu);                                        \
    } while (0)

QMenu *Ui::buildSettingsMenu() {
    QMenu *qmenu = new QMenu("Settings");

    QStringList list = monitor->interfaces();

    // The monitor class could choose an alternative interface string
    // on error. It is important to check the actual value used by the
    // monitor before we create the menu. Post menu creation everything
    // will work as is.
    settings->interface = monitor->getInterface();

    buildSubmenu(
        "Interface",
        settings->interface,
        string,
        interfaceActions,
        selectInterface);

    list.clear();

    list << "Bps" << "Kps" << "Mps" << "Gps";

    buildSubmenu(
        "Units",
        settings->units,
        index,
        unitsActions,
        selectUnits);

    list.clear();

    list << "Bits" << "Si (xB/s)" << "IEC (xiB/s)";

    buildSubmenu(
        "Prefix",
        settings->prefix,
        index,
        prefixActions,
        selectPrefix);

    list.clear();

    list << "Top Left" << "Top Right" << "Bottom Left" << "Bottom Right";

    buildSubmenu(
        "Position",
        settings->position,
        index,
        positionActions,
        selectPosition);

    prefixDisplayAction = new QAction("Display prefix", this);

    prefixDisplayAction->setCheckable(true);

    if (settings->prefixDisplay)
        boldify(prefixDisplayAction, true);

    connect(prefixDisplayAction, SIGNAL(triggered()), this, SLOT(selectPrefixDisplay()));

    qmenu->addAction(prefixDisplayAction);

    QAction *action = new QAction("Font", this);

    connect(action, SIGNAL(triggered()), this, SLOT(selectFont()));

    qmenu->addAction(action);

    action = new QAction("Color", this);

    connect(action, SIGNAL(triggered()), this, SLOT(selectColor()));

    qmenu->addAction(action);

    return qmenu;
}

void Ui::buildMenu() {
    QMenu *submenu;
    QAction *action;

    menu = new QMenu();

    action = new QAction("About", this);

    connect(action, SIGNAL(triggered()), this, SLOT(displayAbout()));

    menu->addAction(action);

    submenu = buildSettingsMenu();

    menu->addMenu(submenu);

    action = new QAction("Quit", this);

    connect(action, SIGNAL(triggered()), qApp, SLOT(quit()));

    menu->addAction(action);
}

void Ui::createTrayIcon() {
    tray = new QSystemTrayIcon(this);

    tray->setContextMenu(menu);

    tray->setIcon(QIcon(MONITOR_SVG));

    tray->show();
}

void Ui::updateLabels() {
    QFont font = settings->fontFamily;

    rxlabel->setFont(font);

    txlabel->setFont(font);

    double size = font.pointSizeF();

    double divisor = 2.0;

    // It gets quite hard to read at smaller pixel levels.
    if (size < 8)
        divisor = 1.0;
    else if (size < 12)
        divisor = 1.5;

    font.setPointSizeF(font.pointSizeF() / divisor);

    rxprefix->setFont(font);

    txprefix->setFont(font);

    QPalette palette;

    palette.setColor(QPalette::WindowText, settings->fontColor);

    rxlabel->setPalette(palette);

    txlabel->setPalette(palette);

    rxprefix->setPalette(palette);

    txprefix->setPalette(palette);

    updatePosition();
}

void Ui::updatePrefix() {
    static const char *array[3][4] = {
        {"b/s", "Kb/s",  "Mb/s",  "Gb/s"},
        {"B/s", "KB/s",  "MB/s",  "GB/s"},
        {"B/s", "KiB/s", "MiB/s", "GiB/s"}
    };

    QString prefix(array[settings->prefix][settings->units]);

    rxprefix->setText(prefix);

    txprefix->setText(prefix);

    if (settings->prefixDisplay) {
        rxprefix->setVisible(true);
        txprefix->setVisible(true);
    }
    else {
        rxprefix->setVisible(false);
        txprefix->setVisible(false);
    }

    updatePosition();
}

void Ui::updatePosition() {
    QDesktopWidget desktop;

    QRect geometry = desktop.availableGeometry();

    adjustSize();

    switch (settings->position) {
    case Settings::TopLeft:
        move(0, 0);
        break;

    case Settings::TopRight:
        move(desktop.width() - width(), 0);
        break;

    case Settings::BottomLeft:
        move(0, geometry.height() - height());
        break;

    case Settings::BottomRight:
        move(geometry.width() - width(), geometry.height() - height());
        break;
    };
}

void Ui::timerEvent(QTimerEvent *e) {
    Q_UNUSED(e);

    Data data = monitor->poll();

    double rxvalue = data.rx;
    double txvalue = data.tx;
    double divisor = 0.0;

    switch (settings->prefix) {
    case Settings::Bits:
        divisor = 1000.0;
        rxvalue *= 8;
        txvalue *= 8;
        break;

    case Settings::Si:
        divisor = 1000.0;
        break;

    case Settings::IEC:
        divisor = 1024.0;
        break;
    }

    switch (settings->units) {
    case Settings::Gps:
        rxvalue /= divisor;
        txvalue /= divisor;
        // Fall through.
    case Settings::Mps:
        rxvalue /= divisor;
        txvalue /= divisor;
        // Fall through.
    case Settings::Kps:
        rxvalue /= divisor;
        txvalue /= divisor;
        break;
    case Settings::Bps:
    default:
        break;
    }

    rxlabel->setText(QString::number(rxvalue, 'f', 2));

    txlabel->setText(QString::number(txvalue, 'f', 2));

    updatePosition();
}

void Ui::contextMenuEvent(QContextMenuEvent *event)
{
    menu->exec(event->globalPos());
}
