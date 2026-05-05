#ifndef OUR_DRIVER_H_
#define OUR_DRIVER_H_

#include <errno.h>
#include <stdint.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

#ifdef __cplusplus
extern "C" {
#endif

struct our_driver_api {
    struct sensor_driver_api sensor_api;
    int (*set_on_value)(const struct device *dev, uint8_t on_value);
};

static inline int our_driver_set_on_value(const struct device *dev, uint8_t on_value)
{
    const struct our_driver_api *api = (const struct our_driver_api *)dev->api;

    if ((api == NULL) || (api->set_on_value == NULL)) {
        return -ENOSYS;
    }

    return api->set_on_value(dev, on_value);
}

#ifdef __cplusplus
}
#endif

#endif /* OUR_DRIVER_H_ */