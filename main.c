#include "network_structures.h"
#include "network_display.h"
#include "network_config.h"
#include "ethernet_frame.h"
#include <stdio.h>

extern void afficher_equipement(equipement_t eq);

int main() {
    // Etape 1:
    // Exemple de station
    station_t s1 = {
        .mac = {{0x54, 0xd6, 0xa6, 0x82, 0xc5, 0x23}},
        .ip = {{130, 79, 80, 21}}
    };
    equipement_t eq1 = {.type = STATION, .data.station = s1};

    // Exemple de switch
    switch_t sw1 = {
        .mac = {{0x01, 0x45, 0x23, 0xa6, 0xf7, 0xab}},
        .nb_ports = 8,
        .priority = 1024
    };
    equipement_t eq2 = {.type = SWITCH, .data.sw = sw1};

    printf("Affichage Equipement 1:\n");
    afficher_equipement(eq1);

    printf("Affichage Equipement 2:\n");
    afficher_equipement(eq2);
    
    // Etape 2:
    if (argc < 2) {
        printf("Usage: %s <fichier_config>\n", argv[0]);
        return 1;
    }
    reseau_t reseau;
    if (charger_reseau(argv[1], &reseau) != 0) {
        printf("Erreur lors du chargement du fichier\n");
        return 1;
    }

    printf("Equipements du réseau :\n");
    for (int i = 0; i < reseau.nb_equipements; i++) {
        printf("%d. ", i);
        afficher_equipement(reseau.equipements[i]);
    }

    printf("\nLiens du réseau :\n");
    for (int i = 0; i < reseau.nb_liens; i++) {
        printf("Lien %d : %d <--> %d (poids %d)\n",
            i, reseau.liens[i].equip1, reseau.liens[i].equip2, reseau.liens[i].poids);
    }

    // Etape 3:
    // Exemple de trame Ethernet
    uint8_t data[] = {0xde, 0xad, 0xbe, 0xef, 0xca, 0xfe};
    ethernet_frame_t trame;
    creer_trame_ethernet(
        &trame,
        reseau.equipements[7].data.station.mac, // source : station 0
        reseau.equipements[0].data.sw.mac,      // dest : switch 0
        0x0800, // IPv4
        data,
        sizeof(data)
    );

    printf("\n--- Test Trame Ethernet ---\n");
    afficher_trame_utilisateur(&trame);
    afficher_trame_hex(&trame);

    // Etape 4:


    return 0;
}