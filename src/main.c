#define NRF_LOG_MODULE_NAME "APP"

#include <stdint.h>
#include <string.h>

#include "fstorage.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "sensor_ble.h"
#include "sensor_gpio.h"
#include "sensor_timer.h"
#include "ble_configuration_service.h"
#include "sleep.h"
#include "feature_config.h"
#include "watchdog.h"
#include "bma400.h"

void main_handle_input_change(uint32_t index, gpio_config_input_digital_t *config)
{
    #if FEATURE_ENABLED(SLEEP_MODE)
        sleep_handle_gpio_event(index, config);
    #endif
    bma400_handle_gpio_event(index, config);
    ble_handle_input_change(index, config);
}

int main(void) {
    (void) NRF_LOG_INIT(NULL);

    NRF_LOG_INFO("starting\n");

    ble_stack_init();

    fs_init();

    timer_init();

    #if FEATURE_ENABLED(SLEEP_MODE)
    sleep_init(ble_disable_rf);
    #endif

    gpio_init(main_handle_input_change);

    watchdog_init();

    ble_init();

    advertising_start();

    ret_code_t err_code = bma400_setup_orientation_detection();
    NRF_LOG_DEBUG("orientation: %d\n", err_code);

    NRF_LOG_INFO("setup done\n");

    // Enter main loop.
    for (;;) {
        // if (NRF_LOG_PROCESS() == false) {
            power_manage();
            watchdog_feed();
            app_sched_execute();
        // }
    }
}
