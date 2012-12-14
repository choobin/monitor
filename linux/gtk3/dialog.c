#include <assert.h>
#include <stdlib.h>

#include <gtk/gtk.h>

#include "monitor.h"
#include "dialog.h"

void
show_about(GtkWidget *widget, gpointer data)
{
    GdkPixbuf *pixbuf;
    GtkWidget *dialog;

    assert(widget);
    assert(data == NULL);

    pixbuf = gdk_pixbuf_new_from_file_at_size(MONITOR_ICON, 150, 150, NULL);

    dialog = gtk_about_dialog_new();

    gtk_about_dialog_set_program_name(
        GTK_ABOUT_DIALOG(dialog),
        MONITOR_NAME);

    gtk_about_dialog_set_version(
        GTK_ABOUT_DIALOG(dialog),
        MONITOR_VERSION);

    gtk_about_dialog_set_copyright(
        GTK_ABOUT_DIALOG(dialog),
        MONITOR_AUTHOR);

    gtk_about_dialog_set_comments(
        GTK_ABOUT_DIALOG(dialog),
        MONITOR_DESCRIPTION);

    gtk_about_dialog_set_website(
        GTK_ABOUT_DIALOG(dialog),
        MONITOR_URL);

    gtk_about_dialog_set_website_label(
        GTK_ABOUT_DIALOG(dialog),
        MONITOR_URL);

    gtk_about_dialog_set_logo(
        GTK_ABOUT_DIALOG(dialog),
        pixbuf);

    gtk_about_dialog_set_license_type(
        GTK_ABOUT_DIALOG(dialog),
        GTK_LICENSE_BSD);

    gtk_dialog_run(GTK_DIALOG(dialog));

    gtk_widget_destroy(dialog);

    g_object_unref(pixbuf);
}

void
select_font(GtkWidget *widget, gpointer data)
{
    PangoFontDescription *description;
    GtkResponseType result;
    GtkWidget *dialog;
    gchar *font_name;

    assert(widget);
    assert(data == NULL);

    dialog = gtk_font_chooser_dialog_new(
        "Select Font",
        GTK_WINDOW(monitor.window));

    gtk_font_chooser_set_font_desc(
        GTK_FONT_CHOOSER(dialog),
        monitor.font.description);

    result = gtk_dialog_run(GTK_DIALOG(dialog));

    if (result == GTK_RESPONSE_OK || result == GTK_RESPONSE_APPLY)
    {
        font_name = gtk_font_chooser_get_font(GTK_FONT_CHOOSER(dialog));

        description = gtk_font_chooser_get_font_desc(GTK_FONT_CHOOSER(dialog));

        if (monitor.font.description != NULL)
            pango_font_description_free(monitor.font.description);

        monitor.font.description = description;

        monitor.font.size =
            pango_font_description_get_size(description) / PANGO_SCALE;

        g_settings_set_string(monitor.settings, "font", font_name);

        g_free(font_name);

        monitor.dimensions.update = TRUE;

        gtk_widget_queue_draw(monitor.window);
    }

    gtk_widget_destroy(dialog);
}

void
select_color(GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog;
    GtkResponseType result;
    gchar *string;

    assert(widget);
    assert(data == NULL);

    dialog = gtk_color_chooser_dialog_new(
        "Select Font Color",
        GTK_WINDOW(monitor.window));

    gtk_color_chooser_set_rgba(
        GTK_COLOR_CHOOSER(dialog),
        &monitor.font.color);

    result = gtk_dialog_run(GTK_DIALOG(dialog));

    if (result == GTK_RESPONSE_OK)
    {
        gtk_color_chooser_get_rgba(
            GTK_COLOR_CHOOSER(dialog),
            &monitor.font.color);

        string = gdk_rgba_to_string(&monitor.font.color);

        g_settings_set_string(monitor.settings, "font-rgba", string);

        g_free(string);

        gtk_widget_queue_draw(monitor.window);
    }

    gtk_widget_destroy(dialog);
}
