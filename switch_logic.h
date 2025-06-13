#ifndef SWITCH_LOGIC_H
#define SWITCH_LOGIC_H

#include "network_structures.h"
#include "ethernet_frame.h"

int switch_apprendre_mac(switch_t *sw, mac_addr_t mac_src, int port);
int switch_rechercher_port(switch_t *sw, mac_addr_t mac_dest);
void afficher_table_mac(switch_t *sw);

#endif