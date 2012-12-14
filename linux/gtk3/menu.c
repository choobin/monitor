#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <gtk/gtk.h>

#include "dialog.h"
#include "menu.h"
#include "monitor.h"
#include "settings.h"
#include "interface.h"
#include "window.h"

#define new_item(menu, label, callback, arg)                \
    item = gtk_menu_item_new_with_label(label);             \
                                                            \
    g_signal_connect(item,                                  \
                     "activate",                            \
                     G_CALLBACK(callback),                  \
                     arg);                                  \
                                                            \
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

static void
boldify(GtkWidget *widget)
{
    GtkWidget *label;
    const char *text;
    gchar *markup;

    assert(widget);

    text = gtk_menu_item_get_label(GTK_MENU_ITEM(widget));

    label = gtk_bin_get_child(GTK_BIN(widget));

    markup = g_markup_printf_escaped("<b>%s</b>", text);

    gtk_label_set_markup(GTK_LABEL(label), markup);

    g_free(markup);
}

static void
select_interface(GtkWidget *widget, gpointer data)
{
    assert(widget);
    assert(data);

    const gchar *string = gtk_menu_item_get_label(GTK_MENU_ITEM(data));

    g_settings_set_string(monitor.settings, "interface", string);
}

static void
add_interface(gpointer interface, gpointer menu)
{
    GtkWidget *item;

    assert(interface);
    assert(menu);

    new_item(menu, (gchar*)interface, select_interface, item);

    if (strcmp(interface, monitor.interface) == 0)
        boldify(item);
}

static void
create_interface_menu(GtkWidget *parent)
{
    GtkWidget *interface;
    GtkWidget *interface_submenu;

    GList *list = NULL;

    assert(parent);

    interface = gtk_menu_item_new_with_label("Interface");

    interface_submenu = gtk_menu_new();

    list = interface_list();

    g_list_foreach(list, add_interface, interface_submenu);

    g_list_free_full(list, g_free);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(interface), interface_submenu);

    gtk_menu_shell_append(GTK_MENU_SHELL(parent), interface);
}

static void
select_placement(GtkWidget *widget, gpointer data)
{
    assert(widget);
    assert(data);

    // The byte offset from the address of the global anonymous monitor
    // struct determines the value of the enumeration ಠ_ಠ.

    monitor.placement = data - (gpointer)&monitor;

    g_settings_set_enum(monitor.settings, "placement", monitor.placement);

    monitor.dimensions.update = TRUE;

    gtk_widget_queue_draw(monitor.window); // Force on_expose_event.
}

static void
create_placement_menu(GtkWidget *parent)
{
    GtkWidget *placement;
    GtkWidget *placement_submenu;
    GtkWidget *item;

    gpointer ptr;
    size_t i;

    const char *labels[nplacements] = {
        "Top left",
        "Top right",
        "Bottom left",
        "Bottom right",
    };

    assert(parent);

    placement = gtk_menu_item_new_with_label("Placement");

    placement_submenu = gtk_menu_new();

    // The byte offset from the address of the global anonymous monitor
    // struct determines the value of the enumeration ಠ_ಠ.

    ptr = (gpointer)&monitor;

    for (i = 0; i < nplacements; i++) {
        new_item(placement_submenu, labels[i], select_placement, ptr + i);

        if (i == monitor.placement)
            boldify(item);
    }

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(placement), placement_submenu);

    gtk_menu_shell_append(GTK_MENU_SHELL(parent), placement);
}

static void
select_exit()
{
    gtk_widget_destroy(monitor.window);
}

void
show_menu()
{
    GtkWidget *menu;
    GtkWidget *settings;
    GtkWidget *settings_submenu;
    GtkWidget *item;

    menu = gtk_menu_new();

    new_item(menu, "About", show_about, NULL);

    settings = gtk_menu_item_new_with_label("Settings");

    settings_submenu = gtk_menu_new();

    create_interface_menu(settings_submenu);

    create_placement_menu(settings_submenu);

    new_item(settings_submenu, "Font", select_font, NULL);

    new_item(settings_submenu, "Color", select_color, NULL);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(settings), settings_submenu);

    gtk_menu_shell_append(GTK_MENU_SHELL(menu), settings);

    new_item(menu, "Exit", select_exit, NULL);

    gtk_widget_show_all(menu);

    gtk_menu_popup(GTK_MENU(menu),
                   NULL, NULL, NULL, NULL, 0,
                   gtk_get_current_event_time());
}
