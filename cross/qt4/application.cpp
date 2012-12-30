#include <QtGui>

#include "application.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    Application application;

    app.exec();

    return EXIT_SUCCESS;
}
