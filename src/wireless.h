#ifndef WIRELESS_H
#define WIRELESS_H

#include "lwip/ip_addr.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "temperature.h"

void send_message (const ip_addr_t* remote_address, const char* message);
void send_temperature (const ip_addr_t* source_address, float* temperature);
void ap_udp_recv_fn(void* arg, struct udp_pcb* recv_pcb, struct pbuf* p, const ip_addr_t* source_addr, u16_t source_port);
void sta_udp_recv_fn(void* arg, struct udp_pcb* recv_pcb, struct pbuf* p, const ip_addr_t* source_addr, u16_t source_port);

#endif