#include "switch_logic.h"
#include <string.h>
#include <stdio.h>

int switch_apprendre_mac(switch_t *sw, mac_addr_t mac_src, int port) {
    for (int i = 0; i < sw->mac_table_size; i++) {
        if (memcmp(sw->mac_table[i].addr, mac_src.addr, MAC_ADDR_LEN) == 0) {
            // Ne change le port que si la MAC est vue sur un port différent
            if (sw->port_table[i] != port) {
                sw->port_table[i] = port;
            }
            return i;
        }
    }
    if (sw->mac_table_size < MAX_PORTS) {
        sw->mac_table[sw->mac_table_size] = mac_src;
        sw->port_table[sw->mac_table_size] = port;
        sw->mac_table_size++;
        return sw->mac_table_size-1;
    }
    return -1; // table pleine
}

int switch_rechercher_port(switch_t *sw, mac_addr_t mac_dest) {
    for (int i = 0; i < sw->mac_table_size; i++) {
        if (memcmp(sw->mac_table[i].addr, mac_dest.addr, MAC_ADDR_LEN) == 0) {
            return sw->port_table[i];
        }
    }
    return -1;
}

void afficher_table_mac(switch_t *sw) {
    printf("Table MAC du switch :\n");
    for (int i = 0; i < sw->mac_table_size; i++) {
        printf("  Port %d : ", sw->port_table[i]);
        for (int j = 0; j < MAC_ADDR_LEN; j++) {
            printf("%02x", sw->mac_table[i].addr[j]);
            if (j < MAC_ADDR_LEN-1) printf(":");
        }
        printf("\n");
    }
}