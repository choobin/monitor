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
const Mainloop = imports.mainloop;

const St = imports.gi.St;
const Gio = imports.gi.Gio;
const GLib = imports.gi.GLib;

const Main = imports.ui.main;
const PanelMenu = imports.ui.panelMenu;
const PopupMenu = imports.ui.popupMenu;

const Extension = imports.misc.extensionUtils.getCurrentExtension();
const Util = imports.misc.util;

const DCONF_SCHEMA = "org.gnome.shell.extensions.network-monitor";

const NetworkMonitor = new Lang.Class({
    Name: "NetworkMonitor",
    Extends: PanelMenu.Button,

    _init: function() {
        this.parent(0.0, "NetworkMonitor", false);

        this.create_ui();

        this.load_schema();

        this.load_settings();

        this.timeout = GLib.timeout_add(
            GLib.PRIORITY_DEFAULT,
            1000,
            Lang.bind(this, this.update_monitor));
    },

    create_ui: function() {
        this.rxlabel = new St.Label();

        this.txlabel = new St.Label({
            style_class: "nm-tx-label-small"
        });

        this.rxprefix = new St.Label({
            style_class: "nm-prefix-label"
        });

        this.txprefix = new St.Label({
            style_class: "nm-prefix-label"
        });

        this.rxicon = new St.Icon({
            style_class: "nm-icon",
            icon_name: "network-transmit-symbolic",
            icon_size: 14
        });

        this.txicon = new St.Icon({
            style_class: "nm-icon",
            icon_name: "network-receive-symbolic",
            icon_size: 14
        });

        let box = new St.BoxLayout();

        box.add_actor(this.rxlabel);

        box.add_actor(this.rxprefix);

        box.add_actor(this.rxicon);

        box.add_actor(this.txlabel);

        box.add_actor(this.txprefix);

        box.add_actor(this.txicon);

        this.actor.add_actor(box);

        let item = new PopupMenu.PopupMenuItem("Settings");

        item.connect("activate", function() {
            Util.trySpawnCommandLine("gnome-shell-extension-prefs " + Extension.uuid);
        });

        this.menu.addMenuItem(item);
    },

    load_schema: function () {
        let directory = Extension.dir.get_child("schemas").get_path();

        let source = Gio.SettingsSchemaSource.new_from_directory(
            directory,
	    Gio.SettingsSchemaSource.get_default(),
	    false);

        let schema = source.lookup(DCONF_SCHEMA, false);

        this.settings = new Gio.Settings({ settings_schema: schema });

        this.settings.connect("changed", Lang.bind(this, this.update_setting));
    },

    guess_interface: function() {
        // Pull the first entry in /proc/net/dev and use that.
        let data = GLib.file_get_contents("/proc/net/dev");

        let line = String(data[1]).split("\n");

        return line[2].split(":")[0].trim(); // :D
    },

    load_settings: function() {
        this.update_setting(null, "interface");

        if (this.interface_name === "unknown") {
            this.interface_name = this.guess_interface();
            this.settings.set_string("interface", this.interface_name);
        }

        this.update_setting(null, "format");
        this.update_setting(null, "prefix");
        this.update_setting(null, "significant-digits");
        this.update_setting(null, "display-rgba");
        this.update_setting(null, "display-icons");
        this.update_setting(null, "hide-when-idle");

        this.rx = 0;
        this.tx = 0;

        this.initialized = false;

        this.update_labels();
    },

    update_setting: function(self, key) {
        if (key === "interface") {
            this.interface_name = this.settings.get_string("interface");

            this.initialized = false; // And we start again.
        }
        else if (key == "format") {
            this.format = this.settings.get_int("format");
        }
        else if (key == "prefix") {
            this.prefix_type = this.settings.get_int("prefix");
        }
        else if (key == "significant-digits") {
            this.significant_digits = this.settings.get_int("significant-digits");
        }
        else if (key == "display-rgba") {
            this.display_color = this.settings.get_string("display-rgba");

            this.update_labels();
        }
        else if (key == "display-icons") {
            this.display_icons = this.settings.get_boolean("display-icons");

            this.update_icons();
        }
        else if (key == "hide-when-idle") {
            this.hide_when_idle = this.settings.get_boolean("hide-when-idle");
        }
    },

    update_tx_width: function(value) {
        let digits = 1;

        while (value > 1.0) {
            value /= 10;
            digits++;
        }

        digits += this.significant_digits;

        // This feels like a bit of a hack, although, effective. I can
        // not seem to find a better method to keep the tx label right
        // aligned and to readjust when its values digits grow or
        // shrink. Either the St and Clutter layouts are not capable
        // of such as thing, or, they are poorly documented and I can
        // not find what I am after, or, I did not spend enough time
        // hacking at it. I am sure it is a mix of everything.

        if (digits < 5) {
            this.txlabel.set_style_class_name("nm-tx-label-small");
        }
        else if (digits < 8) {
            this.txlabel.set_style_class_name("nm-tx-label-meduim");
        }
        else if (digits < 11) {
            this.txlabel.set_style_class_name("nm-tx-label-large");
        }
        else {
            this.txlabel.set_style_class_name("nm-tx-label-ridiculous");
        }
    },

    update_labels: function() {
        let style = "color: " + this.display_color + ";";

        let items = [this.rxlabel, this.rxprefix, this.rxicon,
                     this.txlabel, this.txprefix, this.txicon];

        items.forEach(function(item) {
            item.set_style(style);
        });
    },

    format_label: function(value) {
        let prefix = [["b/s", "Kb/s",  "Mb/s",  "Gb/s",  "Tb/s",  "Pb/s"],
                      ["B/s", "KB/s",  "MB/s",  "GB/s",  "TB/s",  "PB/s"],
                      ["B/s", "KiB/s", "MiB/s", "GiB/s", "TiB/s", "PiB/s"]];

        let divisor = 1000.0; // Bits && Bytes Si.

        if (this.prefix_type == 0) { // Bits.
            value *= 8;
        }
        else if (this.prefix_type == 2) { // Bytes IEC.
            divisor = 1024.0;
        }

        if (this.format == 1) { // K/s.
            value /= divisor;
        }
        else if (this.format == 2) { // M/s.
            value /= divisor;
            value /= divisor;
        }
        else if (this.format == 3) { // G/s.
            value /= divisor;
            value /= divisor;
            value /= divisor;
        }

        let i = 0;

        let tmp = value;

        if (tmp < 1.0)
            i++;

        while (tmp > 1.00) {
            tmp /= 10;
            i++;
        }

        return [value.toFixed(this.significant_digits), prefix[this.prefix_type][this.format]];
    },

    getxx: function(xx) {
        let path = "/sys/class/net/" + this.interface_name + "/statistics/" + xx + "_bytes";

        return GLib.file_get_contents(path);
    },

    update_icons: function() {
        if (this.display_icons) {
            this.rxicon.show();
            this.txicon.show();
        }
        else {
            this.rxicon.hide();
            this.txicon.hide();
        }
    },

    update_monitor: function() {
        let rx = this.getxx("rx");

        let tx = this.getxx("tx");

        let rxint = parseInt(rx[1]);

        let txint = parseInt(tx[1]);

        if (this.hide_when_idle &&
            this.initialized &&
            rxint == this.rx &&
            txint == this.tx) {
            this.rxlabel.set_text("");
            this.txlabel.set_text("");

            this.rxprefix.set_text("");
            this.txprefix.set_text("");

            this.rxicon.hide();
            this.txicon.hide();

            return true;
        }

        this.update_icons();

        // Set the initial values to 0 B/s.
        if (this.initialized == false) {
            this.initialized = true;
            this.rx = rxint;
            this.tx = txint;
        }

        let value, prefix;

        [value, prefix] = this.format_label(rxint - this.rx);

        this.rxlabel.set_text(value);

        this.rxprefix.set_text(prefix);

        [value, prefix] = this.format_label(txint - this.tx);

        this.txlabel.set_text(value);

        this.txprefix.set_text(prefix);

        this.update_tx_width(value);

        this.rx = rxint;

        this.tx = txint;

        return true;
    },

    destroy: function() {
        Mainloop.source_remove(this.timeout);

        PanelMenu.Button.prototype.destroy.call(this);
    }
});

function init() {
}

let monitor;

function enable() {
    monitor = new NetworkMonitor();

    Main.panel.addToStatusArea('monitor', monitor);
}

function disable() {
    monitor.destroy();
}
