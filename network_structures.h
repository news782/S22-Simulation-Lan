#ifndef NETWORK_STRUCTURES_H
#define NETWORK_STRUCTURES_H

#include <stdint.h>

#define MAC_ADDR_LEN 6
#define IP_ADDR_LEN 4
#define MAX_PORTS 16
#define MAX_STATIONS 32
#define MAX_SWITCHES 16

// Structure Adresse MAC
typedef struct {
    uint8_t addr[MAC_ADDR_LEN];
} mac_addr_t;

// Structure Adresse IP
typedef struct {
    uint8_t addr[IP_ADDR_LEN];
} ip_addr_t;

// Structure Station
typedef struct {
    mac_addr_t mac;
    ip_addr_t ip;
} station_t;

// Structure Switch
typedef struct {
    mac_addr_t mac;
    int nb_ports;
    int priority;
    // Table de commutation : association MAC -> port
    mac_addr_t mac_table[MAX_PORTS];
    int port_table[MAX_PORTS];
    int mac_table_size;
} switch_t;

// Enum pour type d'équipement
typedef enum { STATION, SWITCH } equip_type_t;

// Structure Equipement générique (pour le graphe)
typedef struct {
    equip_type_t type;
    union {
        station_t station;
        switch_t sw;
    } data;
} equipement_t;

// Structure pour représenter un lien entre deux équipements
typedef struct {
    int equip1;
    int equip2;
    int poids;
} lien_t;

// Structure Réseau (liste d'équipements et de liens)
typedef struct {
    int nb_equipements;
    equipement_t equipements[MAX_SWITCHES + MAX_STATIONS];
    int nb_liens;
    lien_t liens[128]; // nombre max de liens
} reseau_t;



#endif // NETWORK_STRUCTURES_H