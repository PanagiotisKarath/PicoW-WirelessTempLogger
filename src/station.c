#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "wireless.h"
#include "ff.h"

//Button is connected to GPIO pin 15 (physical pin 20)
#define BUTTON_PIN 15
//Another button to unmount the SD card
#define UNMOUNT_PIN 3

const TCHAR* sta_filename = "s_temp.txt";
//Volatile because they can change at any moment
volatile bool message_button_pressed = false; 
volatile bool unmount_button_pressed = false;
//WIFI_PORT is defined in CMakeLists.txt as 8080
const int port = WIFI_PORT; 

//Single global interrupt callback function, for both buttons.
void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == BUTTON_PIN) {
        message_button_pressed = true;
    } else if (gpio == UNMOUNT_PIN) {
        unmount_button_pressed = true;
    }
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

    //Initialise all standard I/O interfaces
    stdio_init_all();

    //Set up a button on GPIO pin 15 to trigger the interrupt
    gpio_init(BUTTON_PIN); 
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    //Set up the unmount button
    gpio_init(UNMOUNT_PIN);
    gpio_set_dir(UNMOUNT_PIN, GPIO_IN); 
    gpio_pull_up(UNMOUNT_PIN);
    gpio_set_irq_enabled_with_callback(UNMOUNT_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

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

    //Define the access point IP. This is where the station will send messages
    ip4_addr_t remote_address;
    IP4_ADDR(&remote_address, 192, 168, 4, 1);

    //Initialise wireless chip with country (change country accordingly), and 
    //check for errors
    if(cyw43_arch_init_with_country(CYW43_COUNTRY_GREECE)) {
        printf("\nfailed to initialise");
        return 1;
    }

    //Station mode is enabled
    cyw43_arch_enable_sta_mode(); 

    //Get wifi credentials from CMakeLists.txt
    const char *ap_name = WIFI_SSID; 
    const char *password = PASSWORD;
    
    //Attempt to connect to access point's wifi network. Timeout after 30 secs
    if(cyw43_arch_wifi_connect_timeout_ms(ap_name, password, CYW43_AUTH_WPA2_AES_PSK, 30000) != 0){
        printf("\nfailed to connect to access point");
        return 1;
    }

    //Initialise udp module and receive function for the station.
    udp_init(); //udp module is initialised
    struct udp_pcb *udp = udp_new();
    if(!udp) {
        printf("\nfailed to create udp");
    }
    if (ERR_OK != udp_bind(udp, IP4_ADDR_ANY, port)) {
        printf("\nfailed to bind to port %u", port);
    }
    udp_recv(udp, sta_udp_recv_fn, (void*)NULL);
    
    while(true){
        if(message_button_pressed) {
            sleep_ms(250); //Debounce time
            send_message(&remote_address, "temp sense");
            message_button_pressed = false;
        }
        if(unmount_button_pressed == true) {
            printf("\nUnmounting");
            f_unmount("0:");
            break;
        }
        //Poll the cyw43 architecture to process any pending events.
        //Must be called regularly.
        sleep_ms(100); 
        cyw43_arch_poll();
    }

    
    //Turn on the LED so we know the program is done.
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    while(true) {
        sleep_ms(1000);
    }

    return 0;
}