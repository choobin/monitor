/*
Network Monitor
Copyright (C) 2012 Christopher Hoobin (choobin).

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

const Lang = imports.lang;

const Gtk = imports.gi.Gtk;
const Gio = imports.gi.Gio;
const Gdk = imports.gi.Gdk;

const FileUtils = imports.misc.fileUtils;
const Extension = imports.misc.extensionUtils.getCurrentExtension();

const DCONF_SCHEMA = "org.gnome.shell.extensions.network-monitor";

const INTERFACE_PATH = "/sys/class/net";

const MonitorSettings = new Lang.Class({
    Name: "MonitorSettings",

    _init: function() {
        this.container = new Gtk.Grid({
            column_homogeneous: true,
            column_spacing: 10,
            row_homogeneous: false,
            row_spacing: 10
        });

        // Fill the frame created in extensionPrefs/main.js
        // to remove the ugly looking border.
        this.container.set_size_request(600, 400);

        this.create_widgets();

        let dir = Gio.file_new_for_path(INTERFACE_PATH);

        FileUtils.listDirAsync(dir, Lang.bind(this, this.load_interfaces));
    },

    load_interfaces: function(list) {
        for (let i = 0; i < list.length; i++) {
            let name = list[i].get_name();

            // Gtk.ComboBox.append takes a 'id' and 'name' string as
            // arguments. Setting them both to 'name' lets me save the
            // setting and set active item using the 'name' string.
            this.interfaces.append(name, name);
        }

        this.load_schema();

        this.attach_widgets();

        this.bind_widgets();
    },

    load_schema: function () {
        let directory = Extension.dir.get_child("schemas").get_path();

        let source = Gio.SettingsSchemaSource.new_from_directory(
            directory,
	    Gio.SettingsSchemaSource.get_default(),
	    false);

        let schema = source.lookup(DCONF_SCHEMA, false);

        this.settings = new Gio.Settings({ settings_schema: schema });
    },

    create_widgets: function() {
        this.container.attach(new Gtk.Label({
            label: "Interface",
            margin_top: 15, // Note: +5
            margin_left: 10,
            margin_right: 10,
            halign: Gtk.Align.START
        }), 1, 1, 3, 1);

        this.interfaces = new Gtk.ComboBoxText({
            margin_top: 15, // Note: +5
            margin_left: 10,
            margin_right: 10,
            halign: Gtk.Align.FILL
        });

        this.container.attach(new Gtk.Label({
            label: "Display format",
            margin_top: 10,
            margin_left: 10,
            margin_right: 10,
            halign: Gtk.Align.START
        }), 1, 2, 3, 1);

        this.format = new Gtk.ComboBoxText({
            margin_top: 10,
            margin_left: 10,
            margin_right: 10,
            halign: Gtk.Align.FILL
        });

        this.format.append_text("Fixed B/s");
        this.format.append_text("Fixed K/s");
        this.format.append_text("Fixed M/s");
        this.format.append_text("Fixed G/s");

        this.container.attach(new Gtk.Label({
            label: "Unit prefix",
            margin_top: 10,
            margin_left: 10,
            margin_right: 10,
            halign: Gtk.Align.START
        }), 1, 3, 3, 1);

        this.prefix = new Gtk.ComboBoxText({
            margin_top: 10,
            margin_left: 10,
            margin_right: 10,
            halign: Gtk.Align.FILL
        });

        this.prefix.append_text("Bits");
        this.prefix.append_text("Bytes Si (xB/s)");
        this.prefix.append_text("Bytes IEC (xiB/s)");

        this.container.attach(new Gtk.Label({
            label: "Significant digits",
            margin_top: 10,
            margin_left: 10,
            margin_right: 10,
            halign: Gtk.Align.START
        }), 1, 4, 3, 1);

        this.significant_digits = Gtk.SpinButton.new_with_range(0, 10, 1);

        this.significant_digits.set_margin_top(10);
        this.significant_digits.set_margin_left(10);
        this.significant_digits.set_margin_right(10);

        this.significant_digits.set_halign(Gtk.Align.FILL);

        this.container.attach(new Gtk.Label({
            label: "Display color",
            margin_top: 10,
            margin_left: 10,
            margin_right: 10,
            halign: Gtk.Align.START
        }), 1, 5, 3, 1);

        this.color = new Gtk.ColorButton({
            margin_top: 10,
            margin_left: 10,
            margin_right: 10
        });

        this.container.attach(new Gtk.Label({
            label: "Display icons",
            margin_top: 10,
            margin_left: 10,
            margin_right: 10,
            halign: Gtk.Align.START
        }), 1, 6, 3, 1);

        this.display_icons = new Gtk.CheckButton({
            margin_top: 10,
            margin_left: 10,
            margin_right: 10
        });

        this.container.attach(new Gtk.Label({
            label: "Hide when idle",
            margin_top: 10,
            margin_left: 10,
            margin_right: 10,
            halign: Gtk.Align.START
        }), 1, 7, 3, 1);

        this.hide = new Gtk.CheckButton({
            margin_top: 10,
            margin_left: 10,
            margin_right: 10
        });
    },

    attach_widgets: function() {
        this.container.attach(this.interfaces, 4, 1, 2, 1);

        this.container.attach(this.format, 4, 2, 2, 1);

        this.container.attach(this.prefix, 4, 3, 2, 1);

        this.container.attach(this.significant_digits, 4, 4, 2, 1);

        this.container.attach(this.color, 4, 5, 2, 1);

        this.container.attach(this.display_icons, 4, 6, 2, 1);

        this.container.attach(this.hide, 4, 7, 2, 1);

        this.container.show_all();
    },

    bind_widgets: function() {
        this.interfaces.connect("changed", Lang.bind(this, this.update));

        this.interfaces.set_active_id(this.settings.get_string("interface"));

        this.settings.bind("format", this.format, "active", Gio.SettingsBindFlags.NO_SENSITIVITY);

        this.settings.bind("prefix", this.prefix, "active", Gio.SettingsBindFlags.NO_SENSITIVITY);

        this.settings.bind("significant-digits", this.significant_digits, "value", Gio.SettingsBindFlags.NO_SENSITIVITY);

        this.color.connect("color-set", Lang.bind(this, this.update));

        let rgba = new Gdk.RGBA();

        rgba.parse(this.settings.get_string("display-rgba"));

        this.color.set_rgba(rgba);

        this.settings.bind("display-icons", this.display_icons, "active", Gio.SettingsBindFlags.NO_SENSITIVITY);

        this.settings.bind("hide-when-idle", this.hide, "active", Gio.SettingsBindFlags.NO_SENSITIVITY);
    },

    update: function(object) {
        if (object === this.interfaces) {
            this.settings.set_string("interface", this.interfaces.get_active_text());
        }
        else if (object === this.color) {
            let color = this.color.rgba;

            let rgba = color.to_string();

            this.settings.set_string("display-rgba", rgba);
        }
    }
});

function init() {
}

function buildPrefsWidget(){
    let settings = new MonitorSettings();

    return settings.container;
};
