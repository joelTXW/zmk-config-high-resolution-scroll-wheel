#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>
#include <zephyr/logging/log.h>

#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/keymap.h>

LOG_MODULE_REGISTER(highres_sens_persist, CONFIG_ZMK_LOG_LEVEL);

#if IS_ENABLED(CONFIG_HIGHRES_PERSIST_SENSITIVITY_LAYER)

#define HIGHRES_SETTINGS_SUBTREE "highres"
#define HIGHRES_SETTINGS_KEY "highres/sens_layer"

#define HIGHRES_LAYER_DIV2 0
#define HIGHRES_LAYER_DIV4 1
#define HIGHRES_LAYER_DIV8 2
#define HIGHRES_LAYER_DIV16 3

static uint8_t saved_layer = HIGHRES_LAYER_DIV2;
static struct k_work_delayable save_work;

static bool is_sensitivity_layer(uint8_t layer) {
    return layer == HIGHRES_LAYER_DIV2 || layer == HIGHRES_LAYER_DIV4 ||
           layer == HIGHRES_LAYER_DIV8 || layer == HIGHRES_LAYER_DIV16;
}

static void save_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    int err = settings_save_one(HIGHRES_SETTINGS_KEY, &saved_layer, sizeof(saved_layer));
    if (err) {
        LOG_ERR("Failed to save sensitivity layer: %d", err);
    }
}

static int settings_set(const char *name, size_t len, settings_read_cb read_cb, void *cb_arg) {
    const char *next;

    if (!settings_name_steq(name, "sens_layer", &next) || next) {
        return -ENOENT;
    }

    if (len != sizeof(saved_layer)) {
        return -EINVAL;
    }

    int rc = read_cb(cb_arg, &saved_layer, sizeof(saved_layer));
    if (rc < 0) {
        return rc;
    }

    if (!is_sensitivity_layer(saved_layer)) {
        saved_layer = HIGHRES_LAYER_DIV2;
        return -EINVAL;
    }

    return 0;
}

static int settings_commit(void) {
    if (!is_sensitivity_layer(saved_layer)) {
        return 0;
    }

    int err = zmk_keymap_layer_to(saved_layer, true);
    if (err) {
        LOG_WRN("Failed to restore sensitivity layer %d: %d", saved_layer, err);
    }

    return 0;
}

SETTINGS_STATIC_HANDLER_DEFINE(highres_sens, HIGHRES_SETTINGS_SUBTREE, NULL, settings_set,
                               settings_commit, NULL);

static int sens_persist_listener(const zmk_event_t *eh) {
    const struct zmk_layer_state_changed *ev = as_zmk_layer_state_changed(eh);
    if (ev == NULL || !ev->state) {
        return 0;
    }

    if (!is_sensitivity_layer(ev->layer)) {
        return 0;
    }

    if (saved_layer == ev->layer) {
        return 0;
    }

    saved_layer = ev->layer;
    int err = k_work_reschedule(&save_work, K_MSEC(CONFIG_ZMK_SETTINGS_SAVE_DEBOUNCE));
    if (err < 0) {
        LOG_ERR("Failed to schedule sensitivity save: %d", err);
    }

    return 0;
}

ZMK_LISTENER(highres_sens_persist, sens_persist_listener);
ZMK_SUBSCRIPTION(highres_sens_persist, zmk_layer_state_changed);

static int sens_persist_init(void) {
    k_work_init_delayable(&save_work, save_work_handler);
    return 0;
}

SYS_INIT(sens_persist_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#endif
