#ifndef APPLICATION_H
#define APPLICATION_H

#include "settings.h"
#include "monitor.h"
#include "ui.h"

class Application
{
public:
    Application():
        settings(),
        monitor(settings.interface),
        ui(&settings, &monitor) {};

private:
    Settings settings;
    Monitor monitor;
    Ui ui;
};

#endif /* APPLICATION_H */
