#ifndef MONITOR_H
#define MONITOR_H

#include <stdint.h>

#include <gtk/gtk.h>

#define MONITOR_NAME " Monitor "
#define MONITOR_VERSION "1.0"
#define MONITOR_AUTHOR "Copyright © 2012 Christopher Hoobin"
#define MONITOR_DESCRIPTION "A simple lightweight network monitor."
#define MONITOR_URL "https://github.com/choobin/monitor"
#define MONITOR_ICON "monitor.svg"

// XXX Currently the icon is stored in the same place as the binary.
// When creating a proper install script figure out where to place this.
// /usr/local/share/monitor/monitor.svg? or something?

#define TIMEOUT_INTERVAL 1000
#define INTERFACE_STRING_MAX 32
#define BUFSIZ_SMALL 64
#define PADDING 5

typedef struct {
    GtkWidget *window;
    GSettings *settings;

    struct {
        PangoFontDescription *description;
        GdkRGBA color;
        int size;
    } font;

    struct {
        int width;
        int height;
        int update;
    } dimensions;

    enum {
        placement_top_left,
        placement_top_right,
        placement_bottom_left,
        placement_bottom_right,
        nplacements
    } placement;
    struct {
        uint64_t rx_raw;
        uint64_t tx_raw;
        double rx;
        double tx;

    } data;

    char interface[INTERFACE_STRING_MAX];

} Monitor;

extern Monitor monitor;

void
init_monitor(int, char**);

void
poll_monitor();

void
free_monitor();

#endif /* MONITOR_H */
