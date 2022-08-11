#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gptimer"
#include "driver/gpio.h"
#include "esp_log.h"

static void timer_alarm_callback(void)
{
    
}



static void app_main()
{
    //config gp timer
    gpio_pad_select_gpio(18);


    ////
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t config = {
	.clk_src = GPTIMER_CLK_SRC_DEFAULT,
	.direction = GPTIMER_COUNT_UP, 
	.resolution_hz = 1000*1000, //(1MHz , 1 tick = 1us)
	.flags.intr_shared = 0,
    };
    //instance timer
    ESP_ERROR_CHECK(gptimer_new_timer(&config, &gptimer));
    ESP_ERROR_CHECK(gptimer_set_raw_count(gptimer, 0));
    // set alarm action
    gptimer_alarm_config_t alarm_config = {
	.alarm_count = 500000,
	.reload_count = 0,
	.flags.auto_reload_on_alarm = 1,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
    
    //timer callback
    gptimer_event_callbacks_t event_callbacks = {
	.on_alarm =  timer_alarm_callback,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &event_callbacks));
}
