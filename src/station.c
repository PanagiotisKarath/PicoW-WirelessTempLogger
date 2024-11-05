#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "wireless.h"

#define BUTTON_PIN 15 //Button is connected to GPIO pin 15 (physical pin 20)

volatile bool button_pressed = false; //button pressed bool is false by default. Volatile because it can change at any moment
const int port = WIFI_PORT; //WIFI_PORT is defined in CMakeLists.txt as 8080

//Interrupt Service Routine
void gpio_callback(uint gpio, uint32_t events){
    button_pressed = true;
}

int main(){
    //Initialise all standard I/O interfaces
    stdio_init_all();  

    //Set up a button on GPIO pin 15 to trigger the interrupt
    gpio_init(BUTTON_PIN); 
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

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

    //Initialise udp module. No udp_pcb is needed for now, since not receive 
    //callback is defined (for now)  
    udp_init(); //udp module is initialised
    
    while(true){
        //button_pressed is made true by the interrupt service routine
        if(button_pressed == true){
            sleep_ms(250); //debounce time
            send_message(&remote_address, "temp sense");
        }
        sleep_ms(100); //sleep for 100 ms
        cyw43_arch_poll();//Poll the cyw43 architecture to process any pending events. Must be called regularly
    }

    return 0;
}