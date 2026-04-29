#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define LED_NODE DT_ALIAS(app_led)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

int main(void)
{
    if (!gpio_is_ready_dt(&led)) {
        LOG_ERR("LED GPIO not ready");
        return 0;
    }

    if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE) < 0) {
        LOG_ERR("Failed to configure LED GPIO");
        return 0;
    }

    LOG_INF("Heartbeat started — period: %d ms", CONFIG_APP_HEARTBEAT_PERIOD_MS);

    while (1) {
        gpio_pin_set_dt(&led, 1);
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);

        gpio_pin_set_dt(&led, 0);
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
    }

    return 0;
}