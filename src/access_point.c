#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "dhcpserver.h"
#include "filesystem/vfs.h"
#include "temperature.h"
#include "fs_utils.h"
#include "wireless.h"

const int port = WIFI_PORT;

int main() {
    //Initialize all standard I/O interfaces
    stdio_init_all();
    sleep_ms(1000);
    printf("NOW STARTING\n");
    // Initialize the filesystem
    fs_init();
    // Configure I2C to sense temperature
    i2c_temperature_config();

    // Initialise wireless chip with country (change country accordingly), and 
    // check for errors
    if(cyw43_arch_init_with_country(CYW43_COUNTRY_GREECE)) {
        printf("\nfailed to initialise CYW43 chip.");
        return 1;
    }

    // Set to operate in access point mode, with the credentials given below.
    const char *ap_name = WIFI_SSID;
    const char *password = PASSWORD;
    cyw43_arch_enable_ap_mode(ap_name, password, CYW43_AUTH_WPA2_AES_PSK);

    //Gateway IP and Subnet Mask created. Both are needed for the creation of
    //the DHCP server. 
    ip4_addr_t gateway, netmask;
    IP4_ADDR(&gateway, 192, 168, 4, 1);
    IP4_ADDR(&netmask, 255, 255, 255, 0);
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
    
    while(true) {
        cyw43_arch_poll();
        sleep_ms(1);
    }

    return 0;
}