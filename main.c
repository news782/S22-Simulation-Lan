#include "network_structures.h"
#include "network_display.h"
#include "network_config.h"
#include "ethernet_frame.h"
#include "switch_logic.h"
#include <stdio.h>
#include <string.h>

extern void afficher_equipement(equipement_t eq);

// Fonction pour trouver l'index d'une station par son IP
int trouver_station_par_ip(reseau_t *reseau, ip_addr_t ip) {
    for (int i = 0; i < reseau->nb_equipements; i++) {
        if (reseau->equipements[i].type == STATION &&
            memcmp(reseau->equipements[i].data.station.ip.addr, ip.addr, IP_ADDR_LEN) == 0) {
            return i;
        }
    }
    return -1;
}

// Simulation basique d'une trame de station à station
void simuler_trame_station(reseau_t *reseau, int idx_src, int idx_dest) {
    printf("\n--- Simulation d'une trame ---\n");
    station_t src = reseau->equipements[idx_src].data.station;
    station_t dest = reseau->equipements[idx_dest].data.station;
    uint8_t data[] = {0xde, 0xad, 0xbe, 0xef};
    ethernet_frame_t trame;
    creer_trame_ethernet(&trame, src.mac, dest.mac, 0x0800, data, sizeof(data));
    afficher_trame_utilisateur(&trame);

    // Recherche d'un switch connecté à la source (simplifié)
    for (int i = 0; i < reseau->nb_liens; i++) {
        int e1 = reseau->liens[i].equip1, e2 = reseau->liens[i].equip2;
        int sw_idx = -1;
        if (e1 == idx_src && reseau->equipements[e2].type == SWITCH) sw_idx = e2;
        if (e2 == idx_src && reseau->equipements[e1].type == SWITCH) sw_idx = e1;
        if (sw_idx != -1) {
            printf("La trame passe par le switch %d\n", sw_idx);
            // Apprentissage MAC source sur le switch
            switch_t *sw = &reseau->equipements[sw_idx].data.sw;
            switch_apprendre_mac(sw, src.mac, 1); // port simplifié
            // Vérifie si la MAC de destination est connue sur le switch
            int port = switch_rechercher_port(sw, dest.mac);
            if (port == -1) {
                printf("MAC destination inconnue, inonde tous les ports\n");
            } else {
                printf("MAC destination connue, envoie sur port %d\n", port);
            }
        }
    }
    printf("La trame arrive à la station destination !\n");
}

int main(int argc, char *argv[]) {
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

    printf("\n--- Test Simulation ---\n");
    int idx_src = trouver_station_par_ip(&reseau, reseau.equipements[7].data.station.ip);
    int idx_dest = trouver_station_par_ip(&reseau, reseau.equipements[8].data.station.ip);
    simuler_trame_station(&reseau, idx_src, idx_dest);

    
    return 0;
}