#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "wireless.h"
#include "filesystem/vfs.h"

// Button is connected to GPIO pin 15 (physical pin 20)
#define BUTTON_PIN 15

// Volatile because they can change at any moment
volatile bool message_button_pressed = false; 
volatile bool unmount_button_pressed = false;
// WIFI_PORT is defined in CMakeLists.txt as 8080
const int port = WIFI_PORT;


// Single global interrupt callback function, for both buttons.
void gpio_callback(uint gpio, uint32_t events) {
    (void)gpio;
    (void)events;
    message_button_pressed = true;
    /*
    if (gpio == BUTTON_PIN) {
        message_button_pressed = true;
    } else if (gpio == UNMOUNT_PIN) {
        unmount_button_pressed = true;
    }
    */
}


int main() {
    // Initialize all standard I/O interfaces
    stdio_init_all();
    // Innitialize the filesystem
    fs_init();

    // Set up the message sent button
    gpio_init(BUTTON_PIN); 
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    /*
    // Set up the unmount button
    gpio_init(UNMOUNT_PIN);
    gpio_set_dir(UNMOUNT_PIN, GPIO_IN); 
    gpio_pull_up(UNMOUNT_PIN);
    gpio_set_irq_enabled_with_callback(UNMOUNT_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);*/
    
    // Define the access point IP address, messages will be sent there.
    ip4_addr_t remote_address;
    IP4_ADDR(&remote_address, 192, 168, 4, 1);

    // Initialise wireless chip with country (change country accordingly), and 
    // check for errors
    if(cyw43_arch_init_with_country(CYW43_COUNTRY_GREECE)) {
        printf("\nfailed to initialise CYW43 chip.");
        return 1;
    }

    // Enable station mode
    cyw43_arch_enable_sta_mode();

    // Get wifi credentials from CMakeLists.txt
    const char *ap_name = WIFI_SSID;
    const char *password = PASSWORD;

    // Attempt to connect to access point's wifi network. Timeout after 30 secs
    if(cyw43_arch_wifi_connect_timeout_ms(ap_name, password, CYW43_AUTH_WPA2_AES_PSK, 30000) != 0){
        printf("\nfailed to connect to access point");
        return 1;
    }

    // Initialise udp module and receive function for the station. Also check 
    // for errors.
    udp_init();
    struct udp_pcb *udp =udp_new();
    if(!udp) {
        printf("\nfailed to create udp");
    }
    if (ERR_OK != udp_bind(udp, IP4_ADDR_ANY, port)) {
        printf("\nfailed to bind to port %u", port);
    }
    udp_recv(udp, sta_udp_recv_fn, (void*)NULL);

    while (true) {
        if (message_button_pressed) {
            sleep_ms(250); //Debounce time
            send_message(&remote_address, "temp sense");
            message_button_pressed = false;
        }

        // Poll the cyw43 architecture to process any pending events.
        // Must be called regularly.
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