#include "ethernet_frame.h"
#include <stdio.h>
#include <string.h>

// Construction d'une trame Ethernet
void creer_trame_ethernet(
    ethernet_frame_t *trame,
    mac_addr_t src,
    mac_addr_t dest,
    uint16_t type,
    const uint8_t *data,
    uint16_t data_len
) {
    // Preambule (7 octets 10101010)
    for (int i = 0; i < 7; i++) trame->preambule[i] = 0xAA;
    trame->sfd = 0xAB; // 10101011
    trame->dest = dest;
    trame->src = src;
    trame->type = type;
    // Données
    if (data_len > ETHERNET_MAX_DATA) data_len = ETHERNET_MAX_DATA;
    memcpy(trame->data, data, data_len);
    trame->data_len = data_len;
    // Padding si data < 46 octets
    int bourrage_len = (data_len < 46) ? (46 - data_len) : 0;
    memset(trame->bourrage, 0, bourrage_len);
    // FCS fictif (exemple, à calculer pour de vrai)
    trame->fcs = 0xDEADBEEF;
}

// Affichage lisible d'une trame
void afficher_trame_utilisateur(const ethernet_frame_t *trame) {
    printf("Trame Ethernet :\n");
    printf("  Destination : ");
    afficher_mac(&trame->dest);
    printf("\n  Source      : ");
    afficher_mac(&trame->src);
    printf("\n  Type        : 0x%04x\n", trame->type);
    printf("  Data length : %d octets\n", trame->data_len);
    printf("  Data        : ");
    for (int i = 0; i < trame->data_len; i++) printf("%02x ", trame->data[i]);
    printf("\n  FCS         : 0x%08x\n", trame->fcs);
}

// Affichage brut (hexadécimal) de toute la trame
void afficher_trame_hex(const ethernet_frame_t *trame) {
    printf("Trame (hex) :\n");
    for (int i = 0; i < 7; i++) printf("%02x ", trame->preambule[i]);
    printf("%02x ", trame->sfd);
    for (int i = 0; i < MAC_ADDR_LEN; i++) printf("%02x ", trame->dest.addr[i]);
    for (int i = 0; i < MAC_ADDR_LEN; i++) printf("%02x ", trame->src.addr[i]);
    printf("%02x %02x ", (trame->type >> 8) & 0xFF, trame->type & 0xFF);
    for (int i = 0; i < trame->data_len; i++) printf("%02x ", trame->data[i]);
    // Padding éventuel
    int bourrage_len = (trame->data_len < 46) ? (46 - trame->data_len) : 0;
    for (int i = 0; i < bourrage_len; i++) printf("%02x ", trame->bourrage[i]);
    // FCS
    printf("%02x %02x %02x %02x\n",
        (trame->fcs >> 24) & 0xFF,
        (trame->fcs >> 16) & 0xFF,
        (trame->fcs >> 8) & 0xFF,
        trame->fcs & 0xFF);
}

