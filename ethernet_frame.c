#include "ethernet_frame.h"
#include <stdio.h>

// Construction d'une trame Ethernet simple
void creer_trame_ethernet(
    ethernet_frame_t *trame,
    mac_addr_t src,
    mac_addr_t dest,
    uint16_t type,
    const uint8_t *data,
    uint16_t data_len
) {
    int i;
    for (i = 0; i < 7; i++) trame->preambule[i] = 0xAA;
    trame->sfd = 0xAB;
    trame->dest = dest;
    trame->src = src;
    trame->type = type;

    // Copie manuelle des données
    if (data_len > ETHERNET_MAX_DATA) data_len = ETHERNET_MAX_DATA;
    for (i = 0; i < data_len; i++) trame->data[i] = data[i];
    trame->data_len = data_len;

    // Padding si data < 46 octets
    int padding = (data_len < 46) ? (46 - data_len) : 0;
    for (i = 0; i < padding; i++) trame->bourrage[i] = 0;

    // FCS fictif
    trame->fcs = 0xDEADBEEF;
}

// Affiche une trame de façon lisible
void afficher_trame_utilisateur(const ethernet_frame_t *trame) {
    int i;
    printf("Trame Ethernet :\n");
    printf("  Destination : ");
    afficher_mac(trame->dest);
    printf("\n  Source      : ");
    afficher_mac(trame->src);
    printf("\n  Type        : 0x%04x\n", trame->type);
    printf("  Data length : %d octets\n", trame->data_len);
    printf("  Data        : ");
    for (i = 0; i < trame->data_len; i++) printf("%02x ", trame->data[i]);
    printf("\n  FCS         : 0x%08x\n", trame->fcs);
}

// Affiche une MAC au format standard
void afficher_mac(mac_addr_t mac) {
    int i;
    for (i = 0; i < MAC_ADDR_LEN; i++) {
        printf("%02x", mac.addr[i]);
        if (i < MAC_ADDR_LEN-1) printf(":");
    }
}

// Affiche la trame en hexadécimal (brut)
void afficher_trame_hex(const ethernet_frame_t *trame) {
    int i;
    printf("Trame (hex) :\n");
    for (i = 0; i < 7; i++) printf("%02x ", trame->preambule[i]);
    printf("%02x ", trame->sfd);
    for (i = 0; i < MAC_ADDR_LEN; i++) printf("%02x ", trame->dest.addr[i]);
    for (i = 0; i < MAC_ADDR_LEN; i++) printf("%02x ", trame->src.addr[i]);
    printf("%02x %02x ", (trame->type >> 8) & 0xFF, trame->type & 0xFF);
    for (i = 0; i < trame->data_len; i++) printf("%02x ", trame->data[i]);
    int bourrage_len = (trame->data_len < 46) ? (46 - trame->data_len) : 0;
    for (i = 0; i < bourrage_len; i++) printf("%02x ", trame->bourrage[i]);
    printf("%02x %02x %02x %02x\n",
        (trame->fcs >> 24) & 0xFF,
        (trame->fcs >> 16) & 0xFF,
        (trame->fcs >> 8) & 0xFF,
        trame->fcs & 0xFF);
}