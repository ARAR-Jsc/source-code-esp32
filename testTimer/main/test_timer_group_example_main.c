#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/timer.h"
#include "driver/gpio.h"
#include "rom/gpio.h"

static bool IRAM_ATTR timer_isr_callback(void *args)
{
    BaseType_t high_task_awoken = pdFALSE;
    uint64_t countVal = 0;
    //get count value
    countVal = timer_group_get_counter_value_in_isr(0, 0);
    xQueueSendFromISR(args, &countVal, &high_task_awoken);
    return high_task_awoken == pdTRUE;
}


void app_main()
{
    gpio_pad_select_gpio(23);
    gpio_pad_select_gpio(22);
    gpio_set_direction(23, GPIO_MODE_INPUT);
    gpio_set_pull_mode(23, GPIO_PULLUP_ONLY);
    gpio_set_direction(22, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(22, GPIO_PULLDOWN_ONLY);
    gpio_set_level(22, 1);
    uint64_t getCountVal = 0;
    //create a queue
    QueueHandle_t uQueue = xQueueCreate(10, sizeof(QueueHandle_t)); //this queue contain 10 message
    //config timer
    timer_config_t config = {
	.clk_src = TIMER_SRC_CLK_DEFAULT,
	//we need timer_start after timer_init, if TIMER_START , we don't need timer_start after timer_init
	.counter_en = TIMER_PAUSE,
	.alarm_en = TIMER_ALARM_EN,
	.counter_dir = TIMER_COUNT_UP,
	.divider = 80, //equal 1s, default clk_src = 80MHz
	.auto_reload = TIMER_AUTORELOAD_EN,
    };
    timer_group_t group = 0;
    timer_idx_t timer = 0;
    //init timer
    ESP_ERROR_CHECK(timer_init(group, timer, &config));
    //set timer counting value
    timer_set_counter_value(group, timer, 0); //count from 0
    // set timer alram value
    timer_set_alarm_value(group, timer, 500000); //peroid 0.5s
    //add isr callback
    ESP_ERROR_CHECK(timer_enable_intr(group, timer));
    ESP_ERROR_CHECK(timer_isr_callback_add(group, timer, timer_isr_callback, uQueue, 0));
    //start timer
    ESP_ERROR_CHECK(timer_start(group, timer));
    //wait for message from isr
    
    xQueueReceive(uQueue, &getCountVal, portMAX_DELAY);
    printf("Dump value of conuter \"this value is different between auto reload on/off\": %llu\n", getCountVal);
    timer_pause(group, timer),
    timer_set_auto_reload(group, timer, TIMER_AUTORELOAD_DIS);
    timer_start(group, timer);
    xQueueReceive(uQueue, &getCountVal, portMAX_DELAY);
    printf("Dump value of conuter \"this value is different between auto reload on/off\": %llu\n", getCountVal);
    timer_isr_callback_remove(group, timer);
    timer_deinit(group, timer);
    vQueueDelete(uQueue);
    printf("End of app_main\n");
    unsigned char checkWDT = 1;
    while(checkWDT)
    {
	if(gpio_get_level(23) == 0)
	{
	   while(gpio_get_level(23) == 0);
	   gpio_set_level(22, 0);
	   vTaskDelay(1000/portMAX_DELAY);
	   checkWDT = (!(checkWDT) & 0x01);

	}	
	vTaskDelete(NULL);
    }
}

