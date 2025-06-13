#ifndef NETWORK_DISPLAY_H
#define NETWORK_DISPLAY_H

#include "network_structures.h"

void afficher_mac(mac_addr_t mac);
void afficher_ip(ip_addr_t ip);
void afficher_station(station_t s);
void afficher_switch(switch_t sw);
void afficher_equipement(equipement_t eq);

#endif // NETWORK_DISPLAY_H