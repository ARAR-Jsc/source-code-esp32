#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"
#include "driver/gpio.h"
#include "esp_log.h"

bool status = true;
static void timer_alarm_callback(void)
{
   status = !status & 0x01;
   gpio_set_level(18, status); 
}

/* to del_timer, need disable timer before del_timer*/


void app_main()
{
    //config gp timer
    gpio_pad_select_gpio(18);
    gpio_set_direction(18, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(18, GPIO_PULLDOWN_ONLY);


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
	.alarm_count = 1000000,
	.reload_count = 0,
	.flags.auto_reload_on_alarm = 1,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
    
    //timer callback
    gptimer_event_callbacks_t event_callbacks = {
	.on_alarm =  timer_alarm_callback,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &event_callbacks, NULL));

    //enable timer
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}
