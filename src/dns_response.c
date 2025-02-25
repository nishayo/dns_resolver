#include "dns.h"
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

void parse_dns_response(uint8_t *response, int resp_size) {
    dns_header_t *header = (dns_header_t *)response;
    int ancount = ntohs(header->ancount); // Number of answers

    printf("Response ID: %x\n", ntohs(header->id));
    printf("Answer Count: %d\n", ancount);

    if (ancount == 0) {
        printf("No answers received.\n");
        return;
    }

    int offset = sizeof(dns_header_t); // Start after header

    // Skip the question section properly
    while (response[offset] != 0) offset++; // Domain name
    offset += 5; // Skip null byte + QTYPE (2) + QCLASS (2)

    // Loop through all answers
    for (int i = 0; i < ancount; i++) {
        uint16_t name = ntohs(*(uint16_t *)(response + offset));
        offset += 2; // Skip name (often a pointer)

        uint16_t type = ntohs(*(uint16_t *)(response + offset)); // A, AAAA, CNAME, etc.
        offset += 2;
        offset += 2; // Skip Class
        offset += 4; // Skip TTL
        uint16_t rdlength = ntohs(*(uint16_t *)(response + offset));
        offset += 2; // Skip RDLENGTH

        if (type == 1 && rdlength == 4) { // A record (IPv4)
            printf("IPv4 Address: %d.%d.%d.%d\n",
                response[offset], response[offset + 1],
                response[offset + 2], response[offset + 3]);
        } 
        else if (type == 28 && rdlength == 16) { // AAAA record (IPv6)
            char ipv6_str[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, response + offset, ipv6_str, sizeof(ipv6_str));
            printf("IPv6 Address: %s\n", ipv6_str);
        } 
        else if (type == 5) { // CNAME record
            char cname[256] = {0}; // Buffer for CNAME
            int cname_len = 0;

            while (response[offset] != 0) {
                if ((response[offset] & 0xC0) == 0xC0) { // Handle compression // 0xC0 = 11000000
                    offset = ntohs(*(uint16_t *)(response + offset)) & 0x3FFF; // 0x3FFF = 00111111
                } else {
                    uint8_t len = response[offset++];
                    memcpy(cname + cname_len, response + offset, len);
                    cname_len += len;
                    cname[cname_len++] = '.'; // Add dot separator
                    offset += len;
                }
            }

            if (cname_len > 0) cname[cname_len - 1] = '\0'; // Remove trailing dot
            printf("CNAME: %s\n", cname);

            // Send a new query for the resolved CNAME
            uint8_t query[MAX_DNS_PACKET], new_response[MAX_DNS_PACKET];
            int query_size;
            build_dns_query(cname, query, &query_size, 1);
            int new_resp_size = send_dns_query(query, query_size, new_response);
            if (new_resp_size > 0) parse_dns_response(new_response, new_resp_size);
            else printf("Failed to resolve CNAME: %s\n", cname);
        }
        else {
            printf("Unknown record type: %d\n", type);
        }

        offset += rdlength; // Move to next answer
    }
}
