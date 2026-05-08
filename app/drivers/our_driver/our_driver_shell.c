#include <zephyr/shell/shell.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <errno.h>
#include <stdlib.h>
#include "our_driver.h"

static int cmd_set_handler(const struct shell *sh, int argc, char **argv)
{
    char *endptr;
    long value;
    const struct device *driver;

    if (argc != 2) {
        shell_error(sh, "Missing value. Usage: sensor set <0|1>");
        return -EINVAL;
    }

    errno = 0;
    value = strtol(argv[1], &endptr, 10);
    if ((errno != 0) || (endptr == argv[1]) || (*endptr != '\0')) {
        shell_error(sh, "Invalid value '%s'. Expected integer 0 or 1", argv[1]);
        return -EINVAL;
    }

    if ((value < 0) || (value > 1)) {
        shell_error(sh, "Out of range value %ld. Allowed range: 0..1", value);
        return -ERANGE;
    }

    driver = DEVICE_DT_GET(DT_NODELABEL(our_driver0));
    if (!device_is_ready(driver)) {
        shell_error(sh, "our_driver0 is not ready");
        return -ENODEV;
    }

    if (our_driver_set_on_value(driver, (uint8_t)value) < 0) {
        shell_error(sh, "Failed to set driver value to %ld", value);
        return -EIO;
    }

    shell_print(sh, "on_value set to %ld", value);
    return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
    our_driver_subcmd,
    SHELL_CMD_ARG(set, NULL, "Set driver parameter: <0|1>", cmd_set_handler, 2, 0),
    SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensor, &our_driver_subcmd, "Sensor extension commands", NULL);