#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "dhcpserver.h"
#include "sd_card.h"
#include "ff.h"
#include "temperature.h"
#include "filesystem.h"
#include "wireless.h"

#define BUTTON_PIN 3

const int port = WIFI_PORT; //WIFI_PORT is defined in CMakeLists.txt as
                                   //port 8080
const TCHAR* filename = "temp.txt";                                   
volatile bool button_pressed = false; 

void gpio_callback(uint gpio, uint32_t events){
    button_pressed = true;
}

int main(){
    /*
    * Structures concerning the FAT Filesystem are defined below. FRESULT
    * handles most function in the API, however f_printf returns an int, that
    * is why ret is defined. Also a buffer and the name of the file created 
    * are defined here.
    */
    FATFS fs;
    FRESULT fr;    
    int ret;
    char filename[] = "temp.txt";

    stdio_init_all(); //Initialise all standard I/O interfaces
    
    //Initialise a button on a pin. When pressed, this button will trigger an 
    //interrupt, which will cause the unmount of the SD card. In this way, the
    //the safe termination of the program is achieved.
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN); 
    gpio_pull_up(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    //Configure I2C to sense temperature
    i2c_temperature_config();

    /*
    * Initialise SD card with sd_init_driver(). This function returns a boolean
    * variable, true if initialisation was successful, false if not.
    */
    if(!sd_init_driver()){
        printf("ERROR: Initialisation of SD card could not be completed\r\n");
        while(true);
    }

    /*
    * Mount the drive. Third parameter is the mounting option, 1 means force
    * mount now, 0 would be mount on first access to the drive.
    */
    fr = f_mount(&fs, "0:", 1);
    if(fr != FR_OK){
        printf("ERROR: Mount of filesystem was unsuccessful (%d)\r\n", fr);
        while(true);
    }

    //CYW43 wireless chip is initialised with country, change country 
    //accordingly. If initialisation fails, an error message is printed and 
    //an error code is returned.
    if(cyw43_arch_init_with_country(CYW43_COUNTRY_GREECE)) {
        printf("\nfailed to initialise");
        return 1;
    }

    //Pico is set to operate in access point mode, creating wifi network with
    //the credential given below.
    const char *ap_name = WIFI_SSID;
    const char *password = PASSWORD;
    cyw43_arch_enable_ap_mode(ap_name, password, CYW43_AUTH_WPA2_AES_PSK);

    //Gateway IP and Subnet Mask created. Both are needed for the creation of
    //the DHCP server. 
    ip4_addr_t gateway, netmask;
    IP4_ADDR(&gateway, 192, 168, 4, 1);
    IP4_ADDR(&netmask, 255, 255, 255, 0);

    //DHCP server is created. It's responsible for giving out IPs to connected
    //devices
    dhcp_server_t dhcp_server;
    dhcp_server_init(&dhcp_server, &gateway, &netmask);

    //Setting up udp module to transfer messages. First create udp pcb, then 
    //bind it to all local interfaces and the port used and lastly initialise
    //the udp receive callback function
    udp_init();
    struct udp_pcb *udp = udp_new();
    if(!udp) {
        printf("\nfailed to create udp");
    }
    if (ERR_OK != udp_bind(udp, IP4_ADDR_ANY, port)) {
        printf("\nfailed to bind to port %u", port);
    }
    udp_recv(udp, ap_udp_recv_fn, (void*)NULL);

    printf("All set, entering while loop now\r\n");
    while(true) {
        cyw43_arch_poll();
        sleep_ms(1);
        if(button_pressed == true) {
            f_unmount("0:");
            break;
        }
    }

    /*
    * Turn on the LED so we know the program is done.
    */
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    while(true) {
        sleep_ms(1000);
    }

    return 0;
}