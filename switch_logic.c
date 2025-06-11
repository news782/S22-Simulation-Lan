#include "switch_logic.h"
#include <string.h>

// Ajoute ou met à jour l'association MAC->port dans la table du switch
int switch_apprendre_mac(switch_t *sw, mac_addr_t mac_src, int port) {
    for (int i = 0; i < sw->mac_table_size; i++) {
        if (memcmp(sw->mac_table[i].addr, mac_src.addr, MAC_ADDR_LEN) == 0) {
            sw->port_table[i] = port;
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

// Cherche le port associé à la MAC de destination (ou -1 si inconnu)
int switch_rechercher_port(switch_t *sw, mac_addr_t mac_dest) {
    for (int i = 0; i < sw->mac_table_size; i++) {
        if (memcmp(sw->mac_table[i].addr, mac_dest.addr, MAC_ADDR_LEN) == 0) {
            return sw->port_table[i];
        }
    }
    return -1;
}