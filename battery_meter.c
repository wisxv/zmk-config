#include <zmk/behavior.h>
#include <zmk/keymap.h>
#include <zmk/underglow.h>
#include <zmk/split/bluetooth/peripheral.h>
#include <zephyr/kernel.h>

#define WHITE_R 255
#define WHITE_G 255
#define WHITE_B 255
#define MAX_BARS 5
#define DELAY_MS 150

extern uint8_t battery_level_left;
extern uint8_t battery_level_right;

// Порядок индексации от центра к краям для обеих сторон (для Corne: 18 диодов, 0–8 и 9–17)
static const uint8_t left_leds[MAX_BARS] = {4, 3, 5, 2, 6};   // центр → влево и вправо
static const uint8_t right_leds[MAX_BARS] = {13, 12, 14, 11, 15}; // симметрично правой

int behavior_battery_meter_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                                  struct zmk_behavior_binding_event event) {
    uint8_t level = zmk_is_left() ? battery_level_left : battery_level_right;
    uint8_t bars = (level + 19) / 20; // Округление вверх

    const uint8_t *led_map = zmk_is_left() ? left_leds : right_leds;

    // Отключаем все 5 индикаторов сначала
    for (int i = 0; i < MAX_BARS; i++) {
        zmk_underglow_set_pixel_color(led_map[i], 0, 0, 0);
    }

    // Зажигаем нужное количество
    for (int i = 0; i < bars; i++) {
        zmk_underglow_set_pixel_color(led_map[i], WHITE_R, WHITE_G, WHITE_B);
    }

    zmk_underglow_update();
    k_msleep(DELAY_MS);

    // Возврат постоянной подсветки
    for (int i = 0; i < 18; i++) {
        zmk_underglow_set_pixel_color(i, WHITE_R, WHITE_G, WHITE_B);
    }
    zmk_underglow_update();

    return ZMK_BEHAVIOR_OPAQUE;
}

int behavior_battery_meter_keymap_binding_released(struct zmk_behavior_binding *binding,
                                                   struct zmk_behavior_binding_event event) {
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_driver_api = {
    .binding_pressed = behavior_battery_meter_keymap_binding_pressed,
    .binding_released = behavior_battery_meter_keymap_binding_released,
};

DEVICE_DEFINE(battery_meter, "BATTERY_METER", NULL, NULL, NULL, NULL,
              APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
              &behavior_driver_api);
