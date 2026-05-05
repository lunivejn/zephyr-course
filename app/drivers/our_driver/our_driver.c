#include <stdbool.h>
#include <errno.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include "our_driver.h"

#define DT_DRV_COMPAT our_driver

LOG_MODULE_REGISTER(our_driver, LOG_LEVEL_INF);

#if DT_HAS_ALIAS(app_led)
#define OUR_DRIVER_LED_NODE DT_ALIAS(app_led)
#elif DT_HAS_ALIAS(led0)
#define OUR_DRIVER_LED_NODE DT_ALIAS(led0)
#endif

struct our_driver_data {
    bool led_on;
    uint8_t on_value;
};

#if defined(OUR_DRIVER_LED_NODE)
static const struct gpio_dt_spec our_driver_led = GPIO_DT_SPEC_GET(OUR_DRIVER_LED_NODE, gpios);
#endif

static int our_driver_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
    ARG_UNUSED(chan);

#if !defined(OUR_DRIVER_LED_NODE)
    ARG_UNUSED(dev);
    LOG_ERR("No LED alias found (expected app-led or led0)");
    return -ENODEV;
#else
    struct our_driver_data *data = dev->data;

    if ((chan != SENSOR_CHAN_ALL) && (chan != SENSOR_CHAN_AMBIENT_TEMP)) {
        return -ENOTSUP;
    }

    if (gpio_pin_set_dt(&our_driver_led, data->on_value) < 0) {
        LOG_ERR("Failed to turn LED on");
        return -EIO;
    }

    data->led_on = true;

    return 0;
#endif
}

static int our_driver_channel_get(const struct device *dev, enum sensor_channel chan,
                 struct sensor_value *val)
{
    ARG_UNUSED(chan);

#if !defined(OUR_DRIVER_LED_NODE)
    ARG_UNUSED(dev);
    ARG_UNUSED(val);
    LOG_ERR("No LED alias found (expected app-led or led0)");
    return -ENODEV;
#else
    struct our_driver_data *data = dev->data;

    if (gpio_pin_set_dt(&our_driver_led, data->on_value ? 0 : 1) < 0) {
        LOG_ERR("Failed to turn LED off");
        return -EIO;
    }

    data->led_on = false;

    if (val != NULL) {
        val->val1 = data->led_on ? 1 : 0;
        val->val2 = 0;
    }

    return 0;
#endif
}

static int our_driver_set_on_value_impl(const struct device *dev, uint8_t on_value)
{
    struct our_driver_data *data = dev->data;

    if (on_value > 1U) {
        return -EINVAL;
    }

    data->on_value = on_value;
    LOG_INF("Updated on_value to %u", on_value);

    return 0;
}

static const struct our_driver_api our_driver_api = {
    .sensor_api = {
        .sample_fetch = our_driver_sample_fetch,
        .channel_get = our_driver_channel_get,
    },
    .set_on_value = our_driver_set_on_value_impl,
};

static int our_driver_init(const struct device *dev)
{
#if !defined(OUR_DRIVER_LED_NODE)
    ARG_UNUSED(dev);
    LOG_ERR("No LED alias found (expected app-led or led0)");
    return -1;
#else
    struct our_driver_data *data = dev->data;

    if (!gpio_is_ready_dt(&our_driver_led)) {
        LOG_ERR("LED GPIO not ready");
        return -ENODEV;
    }

    if (gpio_pin_configure_dt(&our_driver_led, GPIO_OUTPUT_INACTIVE) < 0) {
        LOG_ERR("Failed to configure LED GPIO");
        return -EIO;
    }

    data->led_on = false;
    data->on_value = 1U;
    LOG_INF("our_driver initialized");

    return 0;
#endif
}

static struct our_driver_data our_driver_data_0;

DEVICE_DT_INST_DEFINE(0, our_driver_init, NULL, &our_driver_data_0, NULL,
              POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY, &our_driver_api);