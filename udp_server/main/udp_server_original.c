/* BSD Socket API Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include "esp_vfs.h"
#include "esp_vfs_fat.h"

#define PORT CONFIG_EXAMPLE_PORT

#define EXAMPLE_ESP_WIFI_SSID      "myssid"
#define EXAMPLE_ESP_WIFI_PASS      "mypassword"
#define EXAMPLE_ESP_WIFI_CHANNEL   1
#define EXAMPLE_MAX_STA_CONN       4

#define EXAMPLE_ESP_MAXIMUM_RETRY  5

#if CONFIG_EXAMPLE_WIFI_AUTH_OPEN
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#elif CONFIG_ESP_WIFI_AUTH_WEP
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP
#elif CONFIG_ESP_WIFI_AUTH_WPA_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WAPI_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK
#endif
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static const char *TAG = "example";

char* wifiCredential = "/spiflash/wifiInfo.txt"; //use for wifiInfo only please

static EventGroupHandle_t s_wifi_event_group;
////typedef struct wifiInfo
////{
////    char wifiSSID[50] = "";
////    char wifiPassword[30] = "";
////};

///vfs_fat
static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;
const char* base_path = "/spiflash";
static int s_retry_num = 0;
////


// modify by TD
////static void nvsStorageInit()
////{
////	esp_err_t err = nvs_flash_init();
////
////	if(err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
////	{
////		ESP_ERROR_CHECK(nvs_flash_erase());
////		err = nvs_flash_init();
////	}
////	ESP_ERROR_CHECK(err);
////}
////
////static esp_err_t savePair_int32_t(int32_t name, int32_t value)
////{
////	esp_err_t err;
////	printf("Opening Non-Volatile Storage (NVS) handle... ");
////	nvs_handle_t myHandle;
////	err = nvs_open("storage", NVS_READWRITE, &myHandle);
////	if(err != ESP_OK)
////	{
////		printf("Open nvs handleing is failed:%s\n", esp_err_to_name(err));
////		return err;
////	}
////	else
////	{
////		name = 0;
////		err = nvs_get_i32(myHandle, name, &name);
////		switch(err)
////		{
////			case ESP_OK:
////				printf("Init... Done\n");
////				break;
////			case ESP_ERR_NVS_NOT_FOUND:
////				printf("the value is not initial yet!\n");
////				break;
////			default:
////				printf("Error (%s) reading\n", esp_err_to_name(err));
////		}
////		name = value;
////		err = nvs_set_i32(myHandle, name, name);
////		printf((err!=ESP_OK)? "Write to nvs failed\n": "Write to nvs OK!\n");
////		err = nvs_commit(myHandle);
////		printf((err!=ESP_OK)? "Commit to nvs failed\n": "Commit to nvs OK!\n");
////		nvs_close(myHandle);
////	
////	}
////}

///storage
static void wearLevellingStorage(char* fileName, char* data)
{
	ESP_LOGI(TAG, "Wear levelling: Mounting FAT filesystem");
	// To mount device we need name of device partition, define base_path
    // and allow format partition in case if it is new one and was not formatted before
	const esp_vfs_fat_mount_config_t mount_config = {
		.max_files = 4,
		.format_if_mount_failed = true,
		.allocation_unit_size = CONFIG_WL_SECTOR_SIZE
	};
	//mount 
	esp_err_t err = esp_vfs_fat_spiflash_mount_rw_wl(base_path, "storage",&mount_config, &s_wl_handle);
	if(err != ESP_OK)
	{
		ESP_LOGI(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
		return;
	}
	//open file
	////FILE *fd = fopen("/spiflash/hello.txt", "wb");
	FILE *fd = fopen(fileName, "wb");
	if(fd == NULL)
	{
		printf("Failed to open file to write\n");
		return;
	}
	//write to file
	fprintf(fd, data);
	fclose(fd);
	ESP_LOGI(TAG, "File written");
	ESP_LOGI(TAG, "Unmounting FAT filesystem");
	ESP_ERROR_CHECK(esp_vfs_fat_spiflash_unmount_rw_wl(base_path, s_wl_handle));
	ESP_LOGI(TAG, "Done");
}

static const char* wearLevellingStorageRead(char* fileName)
{
	
	ESP_LOGI(TAG, "Wear levelling: Mounting FAT filesystem");
	// To mount device we need name of device partition, define base_path
    // and allow format partition in case if it is new one and was not formatted before
	
	/*if this config is done before, do not do it again*/
	
	const esp_vfs_fat_mount_config_t mount_config = {
		.max_files = 4,
		.format_if_mount_failed = true,
		.allocation_unit_size = CONFIG_WL_SECTOR_SIZE
	};
	//mount 
	esp_err_t err = esp_vfs_fat_spiflash_mount_rw_wl(base_path, "storage",&mount_config, &s_wl_handle);
	if(err != ESP_OK)
	{
		ESP_LOGI(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
		return 0;
	}
	//open file
	////FILE *fd = fopen("/spiflash/nextTime.txt", "rb");
	FILE *fd = fopen(fileName, "rb");
	if(fd == NULL)
	{
		printf("Failed to open file to read\n");
		ESP_ERROR_CHECK(esp_vfs_fat_spiflash_unmount_rw_wl(base_path, s_wl_handle));
		return NULL;
	}
	//write to file
	char line[128];
	////char ml[3];
	////char* ldn = NULL;
	////ldn = (char*)malloc(128 * sizeof(char));
	char multiline[3][128];	
	
	////char** multiLines = NULL;
	//////allocated
	////// nen mo file de check so dong hien co trong file truoc khi cap phat
	////multiLines = (char**)malloc(10 * sizeof(char));
	////unsigned char ii;
	////for(unsigned char ii = 0; ii < 0; ii++)
	////{
	////    multiLines[ii] = (char*)malloc(128 * sizeof(char));
	////}
	////while(fgets(&line, sizeof(line), fd) != NULL)
	////{
	////    strcpy(multiLines[i], line);
	////    i++;
	////}
	////for(unsigned char iii = 0; iii < 10; iii++)
	////{
	////    free(multiLines[iii]);
	////}
	////free(multiLines);
	////read file
	
	////fgets(&line, sizeof(line), fd);
	////strcpy(ldn, line);
	///printf("%s\n", ldn);
	///free(ldn);
	////if(ldn == NULL)
	////{
	////    printf(" moa may\n");
	///}
	unsigned char i = 0;
	while(fgets(&line, sizeof(line), fd) != NULL)
	{
	    for(int j = 0; j<=127; j++)
	    {
		multiline[i][j] = line[j];
	    }
	    i++;
	}
	
	//ESP_LOGI(TAG, "first line: %s", multiline[0]);
	//ESP_LOGI(TAG, "second line: %s", multiline[1]);
	//ESP_LOGI(TAG, "third line: %s", multiline[2]);
	fclose(fd);
	ESP_LOGI(TAG, "Read file ...Done");
	ESP_LOGI(TAG, "Unmounting FAT filesystem");
	ESP_ERROR_CHECK(esp_vfs_fat_spiflash_unmount_rw_wl(base_path, s_wl_handle));
	ESP_LOGI(TAG, "Done");
	////strcpy(data, multiLines[i-1]);
	////free(multiLines);
	////multiLines = NULL;
	////strcpy(temp3, multiline[0]);
	///printf("value of first line: %s\n", temp3);
	//return "return  string";
	char* temp = multiline[0];
	return temp;
	//return 'c';
}
///


///wifi AP
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .channel = EXAMPLE_ESP_WIFI_CHANNEL,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            .pmf_cfg = {
                    .required = false,
            },
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS, EXAMPLE_ESP_WIFI_CHANNEL);
}
 
 
static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        ESP_LOGI(TAG, "got gw:" IPSTR, IP2STR(&event->ip_info.gw));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}
void wifi_init_sta(char* SSID, char* PWD)
{
	printf("show valuer of SSID:%s\n", SSID);
	printf("show valuer of PWD:%s\n", PWD);
    uint8_t ssid[32];
   	uint8_t password[64];
	memcpy(ssid, SSID, 32);
	memcpy(password, PWD, 64);
	printf("show valuer of SSID2:%s\n", SSID);
	printf("show valuer of PWD2:%s\n", PWD);
	printf("show valuer of ssid:%s\n", ssid);
	printf("show valuer of password:%s\n", password);
	//strcpy(ssid, SSID);
	//strcpy(password, PWD);
	s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init()); //init LwIP

    ESP_ERROR_CHECK(esp_event_loop_create_default()); // init event
    esp_netif_create_default_wifi_sta(); // init wifi

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg)); // create app task

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
             .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
        },
    };
	memcpy(wifi_config.sta.ssid, ssid, 32);
	memcpy(wifi_config.sta.password, password, 64);
	printf("value of sta.ssid: %s\n", wifi_config.sta.ssid);
	printf("value of sta.ssid: %s\n", wifi_config.sta.password);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
		//remove wifiINFO file
    	wearLevellingStorage(wifiCredential, "wifi is wrong");
		////int removeFileOK = remove(wifiCredential);
		////if(removeFileOK == 0)
		////{
		////	printf("remove file wifiCredential .. done\n");
		////}
		////else
		////{
		////	printf("failed to remove wifiCredential file\n");
		////}
		printf("restarting ESp32....\n");
		esp_restart();

    } else {
    	wearLevellingStorage(wifiCredential, "wifi is wrong");
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}

/////



static void udp_server_task(void *pvParameters)
{
    ESP_LOGI(TAG, "check point 3");
    char rx_buffer[128];
    char addr_str[128];
    int addr_family = (int)pvParameters;
    int ip_protocol = 0;
    struct sockaddr_in6 dest_addr;

    while (1) {
	ESP_LOGI(TAG, "check point 3");

        if (addr_family == AF_INET) {
            struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
            dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
            dest_addr_ip4->sin_family = AF_INET;
            dest_addr_ip4->sin_port = htons(PORT);
            ip_protocol = IPPROTO_IP;
        } else if (addr_family == AF_INET6) {
            bzero(&dest_addr.sin6_addr.un, sizeof(dest_addr.sin6_addr.un));
            dest_addr.sin6_family = AF_INET6;
            dest_addr.sin6_port = htons(PORT);
            ip_protocol = IPPROTO_IPV6;
        }

        int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created");

#if defined(CONFIG_LWIP_NETBUF_RECVINFO) && !defined(CONFIG_EXAMPLE_IPV6)
        int enable = 1;
        lwip_setsockopt(sock, IPPROTO_IP, IP_PKTINFO, &enable, sizeof(enable));
#endif

#if defined(CONFIG_EXAMPLE_IPV4) && defined(CONFIG_EXAMPLE_IPV6)
        if (addr_family == AF_INET6) {
            // Note that by default IPV6 binds to both protocols, it is must be disabled
            // if both protocols used at the same time (used in CI)
            int opt = 1;
            setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
            setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt));
        }
#endif

        int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err < 0) {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        }
        ESP_LOGI(TAG, "Socket bound, port %d", PORT);

        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t socklen = sizeof(source_addr);

#if defined(CONFIG_LWIP_NETBUF_RECVINFO) && !defined(CONFIG_EXAMPLE_IPV6)
        struct iovec iov;
        struct msghdr msg;
        struct cmsghdr *cmsgtmp;
        u8_t cmsg_buf[CMSG_SPACE(sizeof(struct in_pktinfo))];

        iov.iov_base = rx_buffer;
        iov.iov_len = sizeof(rx_buffer);
        msg.msg_control = cmsg_buf;
        msg.msg_controllen = sizeof(cmsg_buf);
        msg.msg_flags = 0;
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        msg.msg_name = (struct sockaddr *)&source_addr;
        msg.msg_namelen = socklen;
#endif

        while (1) {
            ESP_LOGI(TAG, "Waiting for data");
#if defined(CONFIG_LWIP_NETBUF_RECVINFO) && !defined(CONFIG_EXAMPLE_IPV6)
            int len = recvmsg(sock, &msg, 0);
#else
            int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);
#endif
            // Error occurred during receiving
            if (len < 0) {
                ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
                break;
            }
            // Data received
            else {
                // Get the sender's ip address as string
                if (source_addr.ss_family == PF_INET) {
                    inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
#if defined(CONFIG_LWIP_NETBUF_RECVINFO) && !defined(CONFIG_EXAMPLE_IPV6)
                    for ( cmsgtmp = CMSG_FIRSTHDR(&msg); cmsgtmp != NULL; cmsgtmp = CMSG_NXTHDR(&msg, cmsgtmp) ) {
                        if ( cmsgtmp->cmsg_level == IPPROTO_IP && cmsgtmp->cmsg_type == IP_PKTINFO ) {
                            struct in_pktinfo *pktinfo;
                            pktinfo = (struct in_pktinfo*)CMSG_DATA(cmsgtmp);
                            ESP_LOGI(TAG, "dest ip: %s\n", inet_ntoa(pktinfo->ipi_addr));
                        }
                    }
#endif
                } else if (source_addr.ss_family == PF_INET6) {
                    inet6_ntoa_r(((struct sockaddr_in6 *)&source_addr)->sin6_addr, addr_str, sizeof(addr_str) - 1);
                }

                rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string...
                ESP_LOGI(TAG, "Received %d bytes from %s:", len, addr_str);
                ESP_LOGI(TAG, "%s", rx_buffer);
    			wearLevellingStorage(wifiCredential, rx_buffer);
                int err = sendto(sock, rx_buffer, len, 0, (struct sockaddr *)&source_addr, sizeof(source_addr));
                if (err < 0) {
                    ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                    break;
                }
				esp_restart();
            }
        }

        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    printf("start ESP32 ......\n");
	char* wifiINFO=NULL;
    char* SSID=NULL;
    char* PWD=NULL;
    ESP_ERROR_CHECK(nvs_flash_init());
    //wearLevellingStorage("/spiflash/wifiInfo.txt", "Hung Manh_2.4G;15102019;");
    wifiINFO = wearLevellingStorageRead(wifiCredential);
	if(wifiINFO == NULL || strcmp(wifiINFO, "wifi is wrong") == 0)
	{
		printf("not found wifi network, start AP wifi mode\n");
    	wifi_init_softap();
	}
	else
	{
	
		SSID = strsep(&wifiINFO, ";");
		PWD = strsep(&wifiINFO, ";");
		
		////extract wifi info
		if(SSID == NULL || PWD == NULL)
		{
    		wearLevellingStorage(wifiCredential, "wifi is wrong");
			esp_restart();
		}
		printf("SSID : %s\n", SSID);
		printf("PWD : %s\n", PWD);
		
		wifi_init_sta(SSID, PWD);
	}
	////extract wifi info
    //printf("SSID : %s\n", SSID);
    //printf("PWD : %s\n", PWD);
	/////
////	SSID = strsep(&wifiINFO, ";");
////	PWD = strsep(&wifiINFO, ";");
	////ESP_ERROR_CHECK(esp_netif_init());
    ////ESP_ERROR_CHECK(esp_event_loop_create_default());

    ////ESP_ERROR_CHECK(example_connect());
	
	////select wifi mode 
    	//printf("SSID iin2 : %s\n", SSID);
    	//printf("PWD iin2: %s\n", PWD);
#ifdef CONFIG_EXAMPLE_IPV4
    xTaskCreate(udp_server_task, "udp_server", 4096, (void*)AF_INET, 5, NULL);
#endif
#ifdef CONFIG_EXAMPLE_IPV6
    xTaskCreate(udp_server_task, "udp_server", 4096, (void*)AF_INET6, 5, NULL);
#endif

    ////char a = wearLevellingStorageRead("/spiflash/nextTime.txt");
    ////ESP_LOGI(TAG, "%c" ,a);
}
