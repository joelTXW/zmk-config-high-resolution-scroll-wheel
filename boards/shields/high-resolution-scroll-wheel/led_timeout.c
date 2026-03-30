#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/led.h>
#include <zephyr/init.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/event_manager.h>

#define LED_GPIO_NODE_ID DT_COMPAT_GET_ANY_STATUS_OKAY(gpio_leds)
static const uint8_t rgb_idx[] = {DT_NODE_CHILD_IDX(DT_ALIAS(led_red)),
                                  DT_NODE_CHILD_IDX(DT_ALIAS(led_green)),
                                  DT_NODE_CHILD_IDX(DT_ALIAS(led_blue))};

static void turn_off_leds(struct k_work *work) {
    const struct device *led_dev = DEVICE_DT_GET(LED_GPIO_NODE_ID);
    if (!device_is_ready(led_dev)) {
        return;
    }
    
    // Turn off all 3 LEDs (red, green, blue)
    led_off(led_dev, rgb_idx[0]);
    led_off(led_dev, rgb_idx[1]);
    led_off(led_dev, rgb_idx[2]);
}

K_WORK_DELAYABLE_DEFINE(led_timeout_work, turn_off_leds);

static int layer_listener_cb(const zmk_event_t *eh) {
    // When a layer switch occurs, the official widget will update the display.
    // We just set a timer to turn the LEDs off 1.5 seconds later.
    k_work_reschedule(&led_timeout_work, K_MSEC(1500));
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(custom_led_timeout_listener, layer_listener_cb);
ZMK_SUBSCRIPTION(custom_led_timeout_listener, zmk_layer_state_changed);

static int custom_led_timeout_init(void) {
    // On boot, widget blinks battery. We ensure LEDs turn off afterward.
    k_work_reschedule(&led_timeout_work, K_MSEC(3000));
    return 0;
}
SYS_INIT(custom_led_timeout_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

