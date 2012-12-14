#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <gtk/gtk.h>

#include "dialog.h"
#include "menu.h"
#include "monitor.h"
#include "settings.h"
#include "window.h"

static void
update_dimensions(cairo_t *cr)
{
    PangoLayout *layout;
    int width;
    int height;

    assert(cr);

    layout = pango_cairo_create_layout(cr);

    pango_layout_set_font_description(layout, monitor.font.description);

    pango_layout_set_text(layout, "0000.00", -1);

    pango_cairo_update_layout(cr, layout);

    pango_layout_get_size(layout, &width, &height);

    width /= PANGO_SCALE;

    height /= PANGO_SCALE;

    monitor.dimensions.width = width;

    monitor.dimensions.height = (height * 2);

    g_object_unref(layout);

    gtk_window_resize(GTK_WINDOW(monitor.window),
                      monitor.dimensions.width,
                      monitor.dimensions.height);

    switch (monitor.placement) {
    case placement_top_left:
        gtk_window_set_gravity(GTK_WINDOW(monitor.window),
                               GDK_GRAVITY_NORTH_WEST);

        width = PADDING;
        height = PADDING;
        break;

    case placement_top_right:
        gtk_window_set_gravity(GTK_WINDOW(monitor.window),
                               GDK_GRAVITY_NORTH_EAST);

        width = gdk_screen_width();
        height = PADDING;
        break;

    case placement_bottom_left:
        gtk_window_set_gravity(GTK_WINDOW(monitor.window),
                               GDK_GRAVITY_SOUTH_WEST);

        width = PADDING;
        height = gdk_screen_height();
        break;

    case placement_bottom_right:
        gtk_window_set_gravity(GTK_WINDOW(monitor.window),
                               GDK_GRAVITY_SOUTH_EAST);

        width = gdk_screen_width();
        height = gdk_screen_height();
        break;

    default:
        break;
    };

    gtk_window_move(GTK_WINDOW(monitor.window), width, height);

    monitor.dimensions.update = FALSE;

    gtk_widget_queue_draw(monitor.window);
}

static void
pango_text(cairo_t *cr, double value)
{
    PangoLayout *layout;

    char buf[BUFSIZ_SMALL];

    assert(cr);

    layout = pango_cairo_create_layout(cr);

    pango_layout_set_font_description(layout, monitor.font.description);

    snprintf(buf, BUFSIZ_SMALL, "%7.2f", value);

    pango_layout_set_text(layout, buf, -1);

    pango_cairo_update_layout(cr, layout);

    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);
}

static gboolean
draw(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    assert(widget);
    assert(cr);
    assert(data == NULL);

    if (monitor.dimensions.update == TRUE)
        update_dimensions(cr);

    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.0); // 0.0 alpha channel

    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE); // Set source transparent.

    cairo_paint(cr);

    gdk_cairo_set_source_rgba(cr, &monitor.font.color);

    cairo_move_to(cr, 0, 0);

    pango_text(cr, monitor.data.rx);

    cairo_move_to(cr, 0, monitor.font.size + (monitor.font.size / 2));

    pango_text(cr, monitor.data.tx);

    return FALSE;
}

static gboolean
timeout(gpointer data)
{
    assert(data == NULL);

    poll_monitor();

    gtk_widget_queue_draw(monitor.window);

    return TRUE;
}

static gboolean
popup_menu(GtkWidget *widget, guint button, guint atime, gpointer data)
{
    assert(widget);
    assert((button = atime)); // To calm down -Wextra :D.
    assert(data == NULL);

    show_menu();

    return TRUE;
}

static gboolean
button_press(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    assert(widget);
    assert(event);
    assert(data == NULL);

    show_menu();

    return TRUE;
}

static void
close_window(GtkWidget *widget, gpointer data)
{
    assert(widget);
    assert(data == NULL);

    free_monitor();

    gtk_main_quit();
}

void
create_window()
{
    GtkStatusIcon *status_icon;
    GtkWidget *drawing_area;
    GdkVisual *visual;
    GdkScreen *screen;
    GdkPixbuf *icon;

    monitor.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title(GTK_WINDOW(monitor.window), MONITOR_NAME);

    gtk_window_set_default_size(GTK_WINDOW(monitor.window),
                                monitor.dimensions.width,
                                monitor.dimensions.height);

    icon = gdk_pixbuf_new_from_file(MONITOR_ICON, NULL);

    gtk_window_set_icon(GTK_WINDOW(monitor.window), icon);

    g_signal_connect(monitor.window,
                     "destroy",
                     G_CALLBACK(close_window),
                     NULL);

    status_icon = gtk_status_icon_new_from_file(MONITOR_ICON);

    gtk_status_icon_set_title(GTK_STATUS_ICON(status_icon), MONITOR_NAME);

    g_signal_connect(status_icon,
                     "popup-menu",
                     G_CALLBACK(popup_menu),
                     NULL);

    gtk_status_icon_set_visible(status_icon, TRUE);

    screen = gtk_widget_get_screen(monitor.window);

    // Set a rgba color map so we can make the cairo background transparent.
    visual = gdk_screen_get_rgba_visual(screen);
    if (visual == NULL)
        visual = gdk_screen_get_system_visual(screen);

    gtk_widget_set_visual(GTK_WIDGET(monitor.window), visual);

    // Hide window border.
    gtk_window_set_decorated(GTK_WINDOW(monitor.window), FALSE);

    // Show on all desktops.
    gtk_window_stick(GTK_WINDOW(monitor.window));

    // Hide from taskbar.
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(monitor.window), TRUE);

    drawing_area = gtk_drawing_area_new();

    gtk_container_add(GTK_CONTAINER(monitor.window), drawing_area);

    g_signal_connect(drawing_area,
                     "draw",
                     G_CALLBACK(draw),
                     NULL);

    g_timeout_add(TIMEOUT_INTERVAL, (GSourceFunc)timeout, NULL);

    gtk_widget_add_events(GTK_WIDGET(monitor.window),
                          GDK_BUTTON_PRESS_MASK | GDK_CONFIGURE);

    g_signal_connect(monitor.window,
                     "button-press-event",
                     G_CALLBACK(button_press),
                     NULL);

    gtk_widget_show_all(monitor.window);
}
