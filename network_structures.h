#ifndef NETWORK_STRUCTURES_H
#define NETWORK_STRUCTURES_H

#include <stdint.h>

#define MAC_ADDR_LEN 6
#define IP_ADDR_LEN 4
#define MAX_PORTS 64
#define MAX_STATIONS 32
#define MAX_SWITCHES 16

typedef struct {
    uint8_t addr[MAC_ADDR_LEN];
} mac_addr_t;

typedef struct {
    uint8_t addr[IP_ADDR_LEN];
} ip_addr_t;

typedef struct {
    mac_addr_t mac;
    ip_addr_t ip;
} station_t;

typedef struct {
    mac_addr_t mac;
    int nb_ports;
    int priority;
    mac_addr_t mac_table[MAX_PORTS];
    int port_table[MAX_PORTS]; // index du voisin
    int port_etat[MAX_PORTS];  // 1 = actif (spanning tree), 0 = bloqué
    int mac_table_size;
} switch_t;

typedef enum { STATION, SWITCH } equip_type_t;

typedef struct {
    equip_type_t type;
    union {
        station_t station;
        switch_t sw;
    } data;
} equipement_t;

typedef struct {
    int equip1;
    int equip2;
    int poids;
} lien_t;

typedef struct {
    int nb_equipements;
    equipement_t equipements[MAX_SWITCHES + MAX_STATIONS];
    int nb_liens;
    lien_t liens[128];
} reseau_t;

// Fonctions simples de comparaison
int mac_egal(mac_addr_t m1, mac_addr_t m2);
int ip_egal(ip_addr_t ip1, ip_addr_t ip2);

#endif