#include "dns.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <domain>\n", argv[0]);
        return 1;
    }

    uint8_t query[MAX_DNS_PACKET], response[MAX_DNS_PACKET];
    int query_size;

    // Send A record query (IPv4)
    build_dns_query(argv[1], query, &query_size, 1);
    int resp_size = send_dns_query(query, query_size, response);
    if (resp_size > 0) parse_dns_response(response, resp_size);
    else printf("IPv4 DNS query failed.\n");

    // Send AAAA record query (IPv6)
    build_dns_query(argv[1], query, &query_size, 28);
    resp_size = send_dns_query(query, query_size, response);
    if (resp_size > 0) parse_dns_response(response, resp_size);
    else printf("IPv6 DNS query failed.\n");

    return 0;
}
