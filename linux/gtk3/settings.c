#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>

#include "monitor.h"
#include "settings.h"

static void
update_interface()
{
    gchar *string;

    string = g_settings_get_string(monitor.settings, "interface");

    strncpy(monitor.interface, string, INTERFACE_STRING_MAX);

    g_free(string);
}

static void
update_font()
{
    gchar *string;

    string = g_settings_get_string(monitor.settings, "font");

    if (monitor.font.description != NULL)
        pango_font_description_free(monitor.font.description);

    monitor.font.description = pango_font_description_from_string(string);

    monitor.font.size =
        pango_font_description_get_size(monitor.font.description) /
        PANGO_SCALE;

    g_free(string);

    if (monitor.dimensions.update != TRUE) {
        gtk_widget_queue_draw(monitor.window);
        monitor.dimensions.update = TRUE;
    }
}

static void
update_font_rgba()
{
    gchar *string;

    string = g_settings_get_string(monitor.settings, "font-rgba");

    gdk_rgba_parse(&monitor.font.color, string);

    g_free(string);

    if (monitor.dimensions.update != TRUE) {
        gtk_widget_queue_draw(monitor.window);
        monitor.dimensions.update = TRUE;
    }
}

static void
update_placement()
{
    monitor.placement = g_settings_get_enum(monitor.settings, "placement");

    if (monitor.dimensions.update != TRUE) {
        gtk_widget_queue_draw(monitor.window);
        monitor.dimensions.update = TRUE;
    }
}

void
update_settings()
{
    update_interface();
    update_font();
    update_font_rgba();
    update_placement();
}

void
on_settings_changed(GtkSettings *settings, gchar *key, gpointer data)
{
    assert(settings);
    assert(key);
    assert(data == NULL);

    if (g_strcmp0(key, "interface") == 0)
        update_interface();
    else if (g_strcmp0(key, "font") == 0)
        update_font();
    else if (g_strcmp0(key, "font-rgba") == 0)
        update_font_rgba();
    else if (g_strcmp0(key, "placement") == 0)
        update_placement();
}
