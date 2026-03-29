#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>
#include <zephyr/logging/log.h>
#include <zmk/behavior.h>
#include <zmk/keymap.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/event_manager.h>

#ifdef CONFIG_SETTINGS

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static uint8_t saved_layer = 0;

static int sensitivity_settings_set(const char *name, size_t len, settings_read_cb read_cb, void *cb_arg) {
    const char *next;
    if (settings_name_steq(name, "layer", &next) && !next) {
        if (len != sizeof(saved_layer)) {
            return -EINVAL;
        }
        int rc = read_cb(cb_arg, &saved_layer, sizeof(saved_layer));
        if (rc >= 0) {
            LOG_INF("Loaded sensitivity layer %d", saved_layer);
        }
        return rc >= 0 ? 0 : rc;
    }
    return -ENOENT;
}

SETTINGS_STATIC_HANDLER_DEFINE(sensitivity, "sensitivity", NULL, sensitivity_settings_set, NULL, NULL);

static void save_layer_work_handler(struct k_work *work) {
    settings_save_one("sensitivity/layer", &saved_layer, sizeof(saved_layer));
}

static K_WORK_DELAYABLE_DEFINE(save_layer_work, save_layer_work_handler);

static int save_layer_event_handler(const zmk_event_t *eh) {
    uint8_t highest = zmk_keymap_highest_layer_active();
    
    // Only track if the highest active layer is one of our sensitivity layers (0-3)
    if (highest <= 3) {
        if (saved_layer != highest) {
            saved_layer = highest;
            k_work_reschedule(&save_layer_work, K_MSEC(1000));
        }
    }
    return 0;
}

ZMK_LISTENER(save_layer, save_layer_event_handler);
ZMK_SUBSCRIPTION(save_layer, zmk_layer_state_changed);

static int sensitivity_init(void) {
    // Only apply if it's not default layer, as default layer is always on
    if (saved_layer > 0 && saved_layer <= 3) {
        // ZMK changed this API to have 'locking' parameter recently.
        // It appears the build server is using an older ZMK version.
        zmk_keymap_layer_to(saved_layer);
    }
    return 0;
}

SYS_INIT(sensitivity_init, APPLICATION, 99);

#endif /* CONFIG_SETTINGS */

