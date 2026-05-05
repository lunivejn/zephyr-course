#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/sensor.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
	const struct device *driver = DEVICE_DT_GET(DT_NODELABEL(our_driver0));
	struct sensor_value val;

	if (!device_is_ready(driver)) {
		LOG_ERR("our_driver0 is not ready");
		return 0;
	}

	LOG_INF("Blinking LED through our_driver");

	while (1) {
		int ret = sensor_sample_fetch(driver);
		if (ret < 0) {
			LOG_ERR("our_driver sample_fetch failed: %d", ret);
			k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
			continue;
		}

		k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);

		ret = sensor_channel_get(driver, SENSOR_CHAN_ALL, &val);
		if (ret < 0) {
			LOG_ERR("our_driver channel_get failed: %d", ret);
		} else {
			LOG_INF("LED state after channel_get: %d", val.val1);
		}

		k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
	}

	return 0;
}