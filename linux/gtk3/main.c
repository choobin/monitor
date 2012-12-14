#include <stdlib.h>

#include <gtk/gtk.h>

#include "monitor.h"

int main(int argc, char **argv)
{
    gtk_init(&argc, &argv);

    init_monitor(argc, argv);

    gtk_main();

    return EXIT_SUCCESS;
}
