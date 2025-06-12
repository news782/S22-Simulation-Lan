#include "network_structures.h"
#include <stdio.h>

// Affiche une adresse MAC en notation hexadécimale
void afficher_mac(mac_addr_t mac) {
    for (int i = 0; i < MAC_ADDR_LEN; i++) {
        printf("%02x", mac.addr[i]);
        if (i < MAC_ADDR_LEN-1) printf(":");
    }
}

// Affiche une adresse IP en notation décimale pointée
void afficher_ip(ip_addr_t ip) {
    for (int i = 0; i < IP_ADDR_LEN; i++) {
        printf("%d", ip.addr[i]);
        if (i < IP_ADDR_LEN-1) printf(".");
    }
}

// Affichage d'une station
void afficher_station(station_t s) {
    printf("MAC: ");
    afficher_mac(s.mac);
    printf(" | IP: ");
    afficher_ip(s.ip);
    printf("\n");
}

// Affichage d'un switch
void afficher_switch(switch_t sw) {
    printf("MAC: ");
    afficher_mac(sw.mac);
    printf(" | Ports: %d | Priorité: %d\n", sw.nb_ports, sw.priority);
}

// Affichage générique d'un équipement
void afficher_equipement(equipement_t eq) {
    if (eq.type == STATION) {
        printf("Station - ");
        afficher_station(eq.data.station);
    } else {
        printf("Switch - ");
        afficher_switch(eq.data.sw);
    }
}
