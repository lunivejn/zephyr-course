#include <errno.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>

static const struct device *get_driver_or_error(const struct shell *sh)
{
    const struct device *driver = DEVICE_DT_GET(DT_NODELABEL(our_driver0));

    if (!device_is_ready(driver)) {
        shell_error(sh, "our_driver0 is not ready");
        return NULL;
    }

    return driver;
}

static int cmd_fetch_handler(const struct shell *sh, int argc, char **argv)
{
    ARG_UNUSED(argv);
    const struct device *driver;
    int ret;

    if (argc != 1) {
        shell_error(sh, "Usage: sensor fetch");
        return -EINVAL;
    }

    driver = get_driver_or_error(sh);
    if (driver == NULL) {
        return -ENODEV;
    }

    ret = sensor_sample_fetch(driver);
    if (ret < 0) {
        shell_error(sh, "sensor_sample_fetch failed: %d", ret);
        return ret;
    }

    shell_print(sh, "fetch: ok");
    return 0;
}

static int cmd_read_handler(const struct shell *sh, int argc, char **argv)
{
    ARG_UNUSED(argv);
    const struct device *driver;
    struct sensor_value val;
    int ret;

    if (argc != 1) {
        shell_error(sh, "Usage: sensor read");
        return -EINVAL;
    }

    driver = get_driver_or_error(sh);
    if (driver == NULL) {
        return -ENODEV;
    }

    ret = sensor_channel_get(driver, SENSOR_CHAN_ALL, &val);
    if (ret < 0) {
        shell_error(sh, "sensor_channel_get failed: %d", ret);
        return ret;
    }

    shell_print(sh, "read: val1=%d val2=%d", val.val1, val.val2);
    return 0;
}

static int cmd_info_handler(const struct shell *sh, int argc, char **argv)
{
    ARG_UNUSED(argv);
    const struct device *driver = DEVICE_DT_GET(DT_NODELABEL(our_driver0));

    if (argc != 1) {
        shell_error(sh, "Usage: sensor info");
        return -EINVAL;
    }

    shell_print(sh, "device: %s", driver->name);
    shell_print(sh, "ready: %s", device_is_ready(driver) ? "yes" : "no");
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
    our_driver_subcmd,
    SHELL_CMD_ARG(fetch, NULL, "Call sensor_sample_fetch()", cmd_fetch_handler, 1, 0),
    SHELL_CMD_ARG(read, NULL, "Call sensor_channel_get()", cmd_read_handler, 1, 0),
    SHELL_CMD_ARG(info, NULL, "Print device info", cmd_info_handler, 1, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensor, &our_driver_subcmd, "Sensor shell commands", NULL);
