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


typedef struct {
    uint64_t event_count;
}example_queue_element_t;

static bool IRAM_ATTR timer_isr_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    BaseType_t high_task_awoken = pdFALSE;
    QueueHandle_t queue = (QueueHandle_t)user_ctx;
    example_queue_element_t evt = {
    .event_count = edata->count_value,
    };
    xQueueSendToBackFromISR(queue, &evt, &high_task_awoken);
    return high_task_awoken == pdTRUE;  //return 1 if == is true, other if  == false , note true != 0 , false = 0
}


void app_main()
{
    //config gp timer
    gpio_pad_select_gpio(18);
    gpio_set_direction(18, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(18, GPIO_PULLDOWN_ONLY);


    ////
    QueueHandle_t queue = xQueueCreate(10, sizeof(example_queue_element_t)); //due to queue store data for this type
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
    ////gptimer_event_callbacks_t event_callbacks = {
    ////    .on_alarm =  timer_alarm_callback,
    ////};
    gptimer_event_callbacks_t event_callbacks = {
	.on_alarm =  timer_isr_callback,
    };
    ////ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &event_callbacks, queue));
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &event_callbacks, queue));
    uint64_t getCount;
    //enable timer
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
    xQueueReceive(queue, &getCount , 40000/portTICK_PERIOD_MS);
    ////xQueueReceive(queue, &getCount , pdMS_TO_TICKS(2000));
    printf("Dump value of counter: %llu\n", getCount);
}
