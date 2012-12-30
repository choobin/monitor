#pragma once

struct Settings {
    int interface_id;

    int units_id;

    int prefix_id;

    int placement_id;

    struct {
        HFONT family;
        COLORREF color;
    } font;
};

void
update_settings();

void
load_settings();

void
save_settings();
