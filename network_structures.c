#include "network_structures.h"

// Compare deux MAC, retourne 1 si égales, 0 sinon
int mac_egal(mac_addr_t m1, mac_addr_t m2) {
    int i;
    for (i = 0; i < MAC_ADDR_LEN; i++) {
        if (m1.addr[i] != m2.addr[i]) return 0;
    }
    return 1;
}

// Compare deux IP, retourne 1 si égales, 0 sinon
int ip_egal(ip_addr_t ip1, ip_addr_t ip2) {
    int i;
    for (i = 0; i < IP_ADDR_LEN; i++) {
        if (ip1.addr[i] != ip2.addr[i]) return 0;
    }
    return 1;
}