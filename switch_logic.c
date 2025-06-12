#include "switch_logic.h"
#include <stdio.h>

// Ajoute une MAC à la table si pas déjà là, ou met à jour le port si besoin
int switch_apprendre_mac(switch_t *sw, mac_addr_t mac_src, int port) {
    int i;
    for (i = 0; i < sw->mac_table_size; i++) {
        if (mac_egal(sw->mac_table[i], mac_src)) {
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

// Cherche le port associé à une MAC, -1 si inconnu
int switch_rechercher_port(switch_t *sw, mac_addr_t mac_dest) {
    int i;
    for (i = 0; i < sw->mac_table_size; i++) {
        if (mac_egal(sw->mac_table[i], mac_dest)) {
            return sw->port_table[i];
        }
    }
    return -1;
}

// Affiche la table MAC d'un switch
void afficher_table_mac(switch_t *sw) {
    int i, j;
    printf("Table MAC du switch :\n");
    for (i = 0; i < sw->mac_table_size; i++) {
        printf("  Port %d : ", sw->port_table[i]);
        for (j = 0; j < MAC_ADDR_LEN; j++) {
            printf("%02x", sw->mac_table[i].addr[j]);
            if (j < MAC_ADDR_LEN-1) printf(":");
        }
        printf("\n");
    }
}