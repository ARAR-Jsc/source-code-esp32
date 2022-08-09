/* UART Echo Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "sw_serial.h"
#include <string.h>

/**
 * This is an example which echos any data it receives on configured UART back to the sender,
 * with hardware flow control turned off. It does not use UART driver event queue.
 *
 * - Port: configured UART
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: off
 * - Pin assignment: see defines below (See Kconfig)
 */

#define ECHO_TEST_TXD (CONFIG_EXAMPLE_UART_TXD)
#define ECHO_TEST_RXD (CONFIG_EXAMPLE_UART_RXD)
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

#define ECHO_UART_PORT_NUM      (CONFIG_EXAMPLE_UART_PORT_NUM)
#define ECHO_UART_BAUD_RATE     (CONFIG_EXAMPLE_UART_BAUD_RATE)
#define ECHO_TASK_STACK_SIZE    (CONFIG_EXAMPLE_TASK_STACK_SIZE)

#define BUF_SIZE (1024)


//SwSerial *tmp = sw_new(22, 21, false, 512);
//printf("%u\n", tmp->bitTime);
static void sw_uart()
{
    SwSerial *tmp = sw_new(22, 21, false, 512);
}
static void initUart2()
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        ////.baud_rate = ECHO_UART_BAUD_RATE,
        .baud_rate = 3333,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    ESP_ERROR_CHECK(uart_driver_install(ECHO_UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(ECHO_UART_PORT_NUM, &uart_config));
    ////ESP_ERROR_CHECK(uart_set_pin(ECHO_UART_PORT_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));
    ESP_ERROR_CHECK(uart_set_pin(ECHO_UART_PORT_NUM, 4, 5, ECHO_TEST_RTS, ECHO_TEST_CTS));
}


static void initUart1()
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        ////.baud_rate = ECHO_UART_BAUD_RATE,
        .baud_rate = 3333,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
		.rx_flow_ctrl_thresh = 16,
    };
    int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    ESP_ERROR_CHECK(uart_driver_install(1, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(1, &uart_config));
    ////ESP_ERROR_CHECK(uart_set_pin(ECHO_UART_PORT_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));
    ESP_ERROR_CHECK(uart_set_pin(1, 22, 23, ECHO_TEST_RTS, ECHO_TEST_CTS));
}
char data1[16] = {0xEF, 0xEF, 0xEF, 0xEF, 0x0F, 0xEF, 0xEF, 0xEF, 0xEF, 0xEF, 0xEF, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F};
char data2[] ={0xAA, 0xBB};
char data3[16]={};
static void echo_task_read(void *arg)
{


    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
	unsigned int chieuDai;
	printf("received data21: %u\n", strlen((const char*)data));
	////unsigned int length = 16;
    while (1) {
        // Read data from the UART
		//uart_get_buffered_data_len(1, &length);
        int len = uart_read_bytes(1, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        // Write data back to the UART
		//chieuDai = strlen((const char*)data);
		//printf("received data len: %u\n", len);
		//////if(len > 0)
		//////{	
			for( unsigned char cnt = 19; cnt < 40; cnt ++)
			{
				if((data[cnt-4] == 0x0F)&&(data[cnt-5] == 0x0F)&&(data[cnt-6] == 0x0F)&&(data[cnt-7] == 0x0F)&&(data[cnt-10] == 0xEF)&&
				(data[cnt-11] == 0xEF)&&(data[cnt-12] == 0xEF)&&(data[cnt-13] == 0xEF)&&(data[cnt-14] == 0xEF))
				{
					for(unsigned char dem = 0; dem < cnt-4; dem++)
					{
						data3[dem]=data[cnt-19+dem];
					}
					////for(unsigned char dem = 0; dem < cnt-4; dem++)
					////{
					////	printf("%u\n", data3[dem]);
					////}	
					//printf("ket thuc\n");
					break; //thoat khoi for
				}
			}
		//////	vTaskDelay(54 / portTICK_RATE_MS);
		uart_write_bytes(ECHO_UART_PORT_NUM, data3, 16);
		//////	////data[strlen((const char*)data)]='\0';
		//////	////printf("received data: %u\n", chieuDai);
		//////	////printf("received data: %u\n", chieuDai);
		//////	//len = 0;
		vTaskDelay(84 / portTICK_RATE_MS);
		//////}
		//printf("strop_________\n");
		
    }
}


static void echo_task_write(void *arg)
{


    // Configure a temporary buffer for the incoming data
    //uint8_t *data = (uint8_t *) malloc(BUF_SIZE);

    while (1) {
        // Read data from the UART
        ////int len = uart_read_bytes(ECHO_UART_PORT_NUM, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        // Write data back to the UART
////		if(len > 0)
////		{	
////			data[strlen((const char*)data)]='\0';
////			//printf("received data: %s\n", (char*)data);
////			uart_write_bytes(ECHO_UART_PORT_NUM, (const char *) data, len);
////			len = 0;
////		}
		uart_write_bytes(ECHO_UART_PORT_NUM, data1, sizeof(data1));
		
		vTaskDelay(85 / portTICK_RATE_MS);
    }
}

void app_main(void)
{
    //SwSerial *tmp = sw_new(22, 21, false, 512);
    //printf("%u\n", tmp->bitTime);
    initUart1();
    initUart2();
	////xTaskCreate(echo_task_write, "uart_echo_task", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);
	xTaskCreate(echo_task_read, "uart_echo_task", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);
}