#include <string.h>
#include "wireless.h"
#include "lwip/udp.h"
#include "filesystem.h"

extern volatile bool button_pressed;
extern const int port;

void send_message (const ip_addr_t* remote_address, const char* message) {
    //Boolean variable is set to false so it can be pressed again
    button_pressed = false;

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

void ap_udp_recv_fn(void* arg, struct udp_pcb* recv_pcb, struct pbuf* p, const ip_addr_t* source_addr, u16_t source_port) {
    //Print a confirmation message
    printf("Message received\r\n");

    //Cast unused parameters as void to not get warnings
    (void)arg;
    (void)recv_pcb;
    (void)source_addr;
    (void)source_port;

    //Extract message from packet buffer and store it in an array
    char received_message[p->tot_len];
    memcpy(received_message, p->payload, p->tot_len);

    //Access point expects the character string "temp sense" in order to read 
    //the on-board temperature sensor. The received and expected messages are 
    //compared and if they are the same a temperature reading is made.
    if(strcmp(received_message, "temp sense") == 0) {
        float temperature = read_temperature();
        file_write_temperature(temperature);
    }

}