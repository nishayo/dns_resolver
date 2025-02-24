# DNS Query Resolution

DNS query resolvers are owned by ISPs like Google, and they send DNS queries to the **root servers**.  
These queries are then redirected to the **Top-Level Domain (TLD) servers** and eventually to **authoritative name servers**.

## Why DNS Uses UDP Instead of TCP

DNS queries use **UDP** and not TCP because:
- DNS servers handle **millions of requests per second**.
- DNS queries are **small** and typically fit in a **single packet**.
- **No need for connection setup** (TCP handshakes, state handling).
- UDP allows **faster query resolution**.

For DNS queries, the **EDNS0 specification** allows **packet sizes of around 4000 bytes**,  
but I am still using the **typical 512-byte max size**.

## DNS Header Structure

A typical **DNS header** contains the following fields:

| Field   | Size (bits) | Purpose |
|---------|------------|---------|
| `id`    | 16         | **Transaction ID** – Unique number to match responses to queries. |
| `flags` | 16         | **Flags and opcode** – Contains query/response type, error codes, recursion settings, etc. |
| `qdcount` | 16       | **Question Count** – Number of questions in the query section. Usually 1 for simple lookups. |
| `ancount` | 16       | **Answer Count** – Number of answers in the response. |
| `nscount` | 16       | **Authority Record Count** – Number of authoritative name servers in the response. |
| `arcount` | 16       | **Additional Record Count** – Number of extra records (e.g., for optimizations like EDNS0). |


if cname -> req again, if a record -> ip4 addr, if aaaa record -> ipv6 addr
check if i get hte a record and cname together in a response 
multiple answers in responses (cname+a, a+aaaa, a+a)
make a test script for around 100 domain names popular, unpopular, old and new, correct and wrong, 
bit manipulation where