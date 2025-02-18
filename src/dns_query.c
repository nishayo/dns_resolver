#include "dns.h"
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>

void build_dns_query(const char *domain, uint8_t *buffer, int *query_size) {
    dns_header_t *header = (dns_header_t *)buffer; // buffer is currently empty
    memset(header, 0, sizeof(dns_header_t)); // initialize the header struct fields as 0

    header->id = htons(0x1234); // random transaction id 
    header->flags = htons(0x0100); // specifies standard dns query with recursion desired
    header->qdcount = htons(1); // question count is 1

    uint8_t *qname = buffer + sizeof(dns_header_t);
    const char *delim = ".";
    char *token, domain_copy[256];

    // domain name is written in label encoding format in a dns query. 
    // like 'example.com' -> '7example3com0'
    // so this is what we are doing below 
    strcpy(domain_copy, domain);
    token = strtok(domain_copy, delim);
    while (token) {
        uint8_t len = (uint8_t)strlen(token);
        *qname++ = len;
        memcpy(qname, token, len);
        qname += len;
        token = strtok(NULL, delim);
    }
    *qname++ = 0;  // Null terminator

    uint16_t *qtype = (uint16_t *)qname;
    *qtype++ = htons(1);  // A record
    *qtype = htons(1);     // IN class

    *query_size = (qname + sizeof(uint16_t) * 2) - buffer;
}

int send_dns_query(uint8_t *query, int query_size, uint8_t *response) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return -1;

    // struct for storing the DNS server info
    struct sockaddr_in server = {
        .sin_family = AF_INET,
        .sin_port = htons(DNS_PORT)
    };
    inet_pton(AF_INET, DNS_SERVER, &server.sin_addr); // convert DNS server IP addr from string to binary

    sendto(sock, query, query_size, 0, (struct sockaddr *)&server, sizeof(server));
    int resp_size = recvfrom(sock, response, MAX_DNS_PACKET, 0, NULL, NULL);

    close(sock);
    return resp_size;
}
