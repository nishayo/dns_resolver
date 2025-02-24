#ifndef DNS_H
#define DNS_H

#include <stdint.h>

#define DNS_SERVER "8.8.8.8" // Google's publically accessbile dns (root)server ip
#define DNS_PORT 53 // default port for dns queries 
#define MAX_DNS_PACKET 512 // udp packet max length allowed

// DNS header struct to send with DNS query
typedef struct {
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
} __attribute__((packed)) dns_header_t; // specified to no padding between fields of the struct 

void build_dns_query(const char *domain, uint8_t *buffer, int *query_size);
int send_dns_query(uint8_t *query, int query_size, uint8_t *response);
void parse_dns_response(uint8_t *response, int resp_size);

#endif