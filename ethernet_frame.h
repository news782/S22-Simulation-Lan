#ifndef ETHERNET_FRAME_H
#define ETHERNET_FRAME_H

#include <stdint.h>
#include "network_structures.h"

#define ETHERNET_MAX_DATA 1500

typedef struct {
    uint8_t preambule[7];      // 7 octets = préambule
    uint8_t sfd;               // 1 octet = start frame delimiter
    mac_addr_t dest;           // 6 octets
    mac_addr_t src;            // 6 octets
    uint16_t type;             // 2 octets (ex: 0x0800 pour IPv4)
    uint8_t data[ETHERNET_MAX_DATA]; // données utiles (0 à 1500 octets)
    uint16_t data_len;         // longueur réelle des données
    uint8_t bourrage[46];      // padding si data < 46 octets
    uint32_t fcs;              // 4 octets (simplifié ici)
} ethernet_frame_t;

void creer_trame_ethernet(
    ethernet_frame_t *trame,
    mac_addr_t src,
    mac_addr_t dest,
    uint16_t type,
    const uint8_t *data,
    uint16_t data_len
);

void afficher_trame_utilisateur(const ethernet_frame_t *trame);
void afficher_trame_hex(const ethernet_frame_t *trame);
void afficher_mac(mac_addr_t mac);

#endif