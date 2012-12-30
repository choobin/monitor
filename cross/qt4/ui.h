#ifndef UI_H
#define UI_H

#include <QtGui>

#include "monitor.h"
#include "settings.h"

class Ui : public QWidget
{
    Q_OBJECT

public:
    Ui(Settings*, Monitor*, QWidget *parent = 0);

protected:
    void timerEvent(QTimerEvent*);

    void contextMenuEvent(QContextMenuEvent*);

private slots:
    void selectInterface(int);

    void selectUnits(int);

    void selectPrefix(int);

    void selectPosition(int);

    void selectFont();

    void selectColor();

    void selectPrefixDisplay();

    void displayAbout();

private:
    Settings *settings;

    Monitor *monitor;

    QLabel *rxlabel;

    QLabel *txlabel;

    QLabel *rxprefix;

    QLabel *txprefix;

    QMenu *menu;

    QSystemTrayIcon *tray;

    QVector<QAction*> interfaceActions;

    QVector<QAction*> unitsActions;

    QVector<QAction*> prefixActions;

    QVector<QAction*> positionActions;

    QAction *prefixDisplayAction;

    void createWidgets();

    void boldify(QAction*, bool=true);

    QMenu *buildSettingsMenu();

    void buildMenu();

    void createTrayIcon();

    void updateLabels();

    void updatePrefix();

    void updatePosition();
};

#endif /* UI_H */
