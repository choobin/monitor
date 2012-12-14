#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <gtk/gtk.h>

#include "dialog.h"
#include "interface.h"
#include "monitor.h"
#include "settings.h"
#include "window.h"

Monitor monitor;

static uint64_t
read_value(const char *path, const char *interface)
{
    FILE *fp;
    char buf[BUFSIZ];
    uint64_t value;
    size_t nbytes;
    char *ptr;

    assert(path);
    assert(interface);

    snprintf(buf, BUFSIZ, path, interface);

    fp = fopen(buf, "r");
    assert(fp);

    nbytes = fread(buf, 1, BUFSIZ, fp);
    assert(nbytes > 0);

    buf[nbytes] = '\0';

    if (buf[nbytes - 1] == '\n')
        buf[nbytes - 1] = '\0';

    value = strtoull(buf, &ptr, 10);
    assert(*ptr == '\0');

    fclose(fp);

    return value;
}

#define RX_PATH "/sys/class/net/%s/statistics/rx_bytes"
#define TX_PATH "/sys/class/net/%s/statistics/tx_bytes"

static void
print_interface(gpointer interface, gpointer data)
{
    assert(interface);
    assert(data == NULL);

    printf("%s\n", (char*)interface);
}

void
init_monitor(int argc, char **argv)
{
    GList *list = NULL;

    list = interface_list();

    if (argc == 2) {
        if (strcmp(argv[1], "-l") == 0 ||
            strcmp(argv[1], "--list") == 0) {
            g_list_foreach(list, print_interface, NULL);
            exit(EXIT_SUCCESS);
        }
    }

    create_window();

    monitor.settings = g_settings_new("apps.monitor");
    assert(monitor.settings);

    g_signal_connect(monitor.settings,
                     "changed",
                     G_CALLBACK(on_settings_changed),
                     NULL);

    update_settings();

    if (strcmp(monitor.interface, "unknown") == 0) {
        if (argc != 2) {
            printf("usage: monitor [ INTERFACE | --list]\n");
            exit(EXIT_FAILURE);
        }

        if (g_list_find_custom(list, argv[1], (GCompareFunc)strcmp) == NULL) {
            printf("Error: The interface '%s' does not exist\n", argv[1]);
            exit(EXIT_FAILURE);
        }

        strncpy(monitor.interface, argv[1], INTERFACE_STRING_MAX);

        g_settings_set_string(monitor.settings, "interface", argv[1]);
    }

    g_list_free_full(list, g_free);

    monitor.data.rx_raw = read_value(RX_PATH, monitor.interface);
    monitor.data.tx_raw = read_value(TX_PATH, monitor.interface);

    monitor.data.rx = 0.0;
    monitor.data.tx = 0.0;
}

void
poll_monitor()
{
    uint64_t rx_raw = read_value(RX_PATH, monitor.interface);
    uint64_t tx_raw = read_value(TX_PATH, monitor.interface);

    monitor.data.rx = (rx_raw - monitor.data.rx_raw) / 1024.0;
    monitor.data.tx = (tx_raw - monitor.data.tx_raw) / 1024.0;

    monitor.data.rx_raw = rx_raw;
    monitor.data.tx_raw = tx_raw;
}

void
free_monitor()
{
    if (monitor.settings)
        g_object_unref(monitor.settings);

    if (monitor.font.description)
        pango_font_description_free(monitor.font.description);
}
