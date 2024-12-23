#include <stdio.h>
#include <string.h>
#include "wireless.h"
#include "lwip/udp.h"
#include "ff.h"
#include "filesystem.h"

extern const TCHAR* ap_filename;
extern const TCHAR* sta_filename;
extern volatile bool message_button_pressed;
extern const int port;

void send_message(const ip_addr_t* remote_address, const char* message) {
    //Boolean variable is set to false so it can be pressed again
    message_button_pressed = false;

    //Create the packet buffer and make it carry the message
    int message_length = strlen(message) + 1;
    struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, message_length, PBUF_RAM);
    memcpy(p->payload, message, message_length);

    //Create the udp pcb, bind it, connect to access point's IP address, send 
    //the message and check for potential errors
    struct udp_pcb* pcb = udp_new();
    udp_bind(pcb, IP_ADDR_ANY, port);
    udp_connect(pcb, remote_address, port);
    int err = udp_send(pcb, p);
    if (err != ERR_OK) {
        printf("\nERROR: message could not be sent (%d)", err);
    }
    
    //Deallocate the memory used for creating the pbuf and the udp pcb
    pbuf_free(p);
    udp_remove(pcb);
}

void send_temperature(const ip_addr_t* source_address, float* temperature) {
    //Create packet buffer and make it carry the temperature variable
    struct pbuf* p = pbuf_alloc(PBUF_TRANSPORT, sizeof(float), PBUF_RAM);
    memcpy(p->payload, temperature, sizeof(float));

    //Create the udp pcb, bind it, connect to station's IP address, send the 
    //message and check for potential errors
    struct udp_pcb* pcb = udp_new();
    udp_bind(pcb, IP_ADDR_ANY, port);
    udp_connect(pcb, source_address, port);
    int err = udp_send(pcb, p);
    if (err != ERR_OK) {
        printf("\nERROR: message could not be sent (%d)");
    } 
    
    //Deallocate the memory used for creating the pbuf and the udp pcb
    pbuf_free(p);
    udp_remove(pcb);
}

void ap_udp_recv_fn(void* arg, struct udp_pcb* recv_pcb, struct pbuf* p, const ip_addr_t* source_addr, u16_t source_port) {
    //Print a confirmation message
    printf("Message received\r\n");

    //Cast unused parameters as void to not get warnings
    (void)arg;
    (void)recv_pcb;
    (void)source_port;

    //Extract message from packet buffer and store it in an array
    char received_message[p->tot_len];
    memcpy(received_message, p->payload, p->tot_len);

    //Access point expects the character string "temp sense" in order to read 
    //the on-board temperature sensor. The received and expected messages are 
    //compared and if they are the same a temperature reading is made.
    if(strcmp(received_message, "temp sense") == 0) {
        float temperature = read_temperature();
        file_write_temperature(temperature, ap_filename);
        send_temperature(source_addr, &temperature);
    }
}

void sta_udp_recv_fn(void* arg, struct udp_pcb* recv_pcb, struct pbuf* p, const ip_addr_t* source_addr, u16_t source_port) {
    //Confirmation message
    printf("Temperature received\r\n");

    //Cast unused parameters as void to not get warnings
    (void)arg;
    (void)recv_pcb;
    (void)source_addr;
    (void)source_port;

    //Extract temperature from packet buffer, store it in a variable
    float remote_temp;
    memcpy(&remote_temp, p->payload, sizeof(float));

    //Store temperature in SD card
    file_write_temperature(remote_temp, sta_filename);
}