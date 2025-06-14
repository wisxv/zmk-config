#include <zmk/events/battery_level_changed.h>

uint8_t battery_level_left = 100;
uint8_t battery_level_right = 100;

int battery_listener(const struct zmk_event_header *eh) {
    const struct battery_level_changed *ev = as_battery_level_changed(eh);

#if IS_ENABLED(CONFIG_ZMK_SPLIT)
    if (zmk_is_left()) {
        battery_level_left = ev->level;
    } else {
        battery_level_right = ev->level;
    }
#else
    battery_level_left = ev->level;
    battery_level_right = ev->level;
#endif

    return 0;
}

ZMK_LISTENER(battery_listener, battery_listener);
ZMK_SUBSCRIPTION(battery_listener, battery_level_changed);
