#include "network_structures.h"
#include "network_display.h"
#include "network_config.h"
#include "ethernet_frame.h"
#include "switch_logic.h"
#include "stp.h"
#include <stdio.h>

#define NB_MAX_EQUIPEMENTS 64

// Recherche d'une station par IP
int trouver_station_par_ip(reseau_t *reseau, ip_addr_t ip) {
    int i;
    for (i = 0; i < reseau->nb_equipements; i++) {
        if (reseau->equipements[i].type == STATION &&
            ip_egal(reseau->equipements[i].data.station.ip, ip)) {
            return i;
        }
    }
    return -1;
}

// Propagation de la trame avec anti-boucle simple
int propager_trame(
    reseau_t *reseau,
    int courant,
    int precedent,
    const ethernet_frame_t *trame,
    int dest_station,
    int *trouve,
    int profondeur,
    int *visited
) {
    if (profondeur > reseau->nb_equipements) return 0;
    if (*trouve) return 1;
    if (visited[courant]) return 0;
    visited[courant] = 1;

    if (courant == dest_station) {
        printf("La trame arrive à la station destination (%d) !\n", courant);
        *trouve = 1;
        return 1;
    }

    equipement_t *eq = &reseau->equipements[courant];
    if (eq->type == SWITCH) {
        switch_t *sw = &eq->data.sw;
        if (precedent != -1) {
            int port_enregistre = switch_rechercher_port(sw, trame->src);
            if (port_enregistre == -1) {
                printf("Switch %d apprend MAC source : ", courant);
                afficher_mac(trame->src);
                printf(" sur port (voisin) %d\n", precedent);
                switch_apprendre_mac(sw, trame->src, precedent);
            }
        }

        int port_equip = switch_rechercher_port(sw, trame->dest);
        if (port_equip != -1 && port_equip != precedent) {
            printf("Switch %d : MAC destination connue, envoie vers équipement %d\n", courant, port_equip);
            propager_trame(reseau, port_equip, courant, trame, dest_station, trouve, profondeur+1, visited);
            return 1;
        } else {
            printf("Switch %d : MAC destination inconnue, inonde tous les ports\n", courant);
            int i;
            for (i = 0; i < reseau->nb_liens; i++) {
                int e1 = reseau->liens[i].equip1, e2 = reseau->liens[i].equip2;
                int voisin = (e1 == courant) ? e2 : (e2 == courant ? e1 : -1);
                if (voisin != -1 && voisin != precedent) {
                    propager_trame(reseau, voisin, courant, trame, dest_station, trouve, profondeur+1, visited);
                    if (*trouve) return 1;
                }
            }
        }
    }
    return 0;
}

// Simulation d'une trame entre deux stations
void simuler_trame_station(reseau_t *reseau, int idx_src, int idx_dest) {
    printf("\n--- Simulation d'une trame de la station %d vers la station %d ---\n", idx_src, idx_dest);

    station_t src = reseau->equipements[idx_src].data.station;
    station_t dest = reseau->equipements[idx_dest].data.station;
    uint8_t data[] = {0xde, 0xad, 0xbe, 0xef};
    ethernet_frame_t trame;
    creer_trame_ethernet(&trame, src.mac, dest.mac, 0x0800, data, sizeof(data));
    afficher_trame_utilisateur(&trame);

    int trouve = 0;
    int visited[NB_MAX_EQUIPEMENTS] = {0};
    int i;
    for (i = 0; i < reseau->nb_liens; i++) {
        int e1 = reseau->liens[i].equip1, e2 = reseau->liens[i].equip2;
        int voisin = (e1 == idx_src) ? e2 : (e2 == idx_src ? e1 : -1);
        if (voisin != -1) {
            propager_trame(reseau, voisin, idx_src, &trame, idx_dest, &trouve, 1, visited);
            if (trouve) break;
        }
    }
    if (!trouve) {
        printf("La trame n'a pas pu atteindre la station destination...\n");
    }
}

int main(int argc, char *argv[]) {
     if (argc < 2) {
        printf("Usage: %s <fichier_config>\n", argv[0]);
        return 1;
    }
    reseau_t reseau;
    if (charger_reseau(argv[1], &reseau) != 0) {
        printf("Erreur lors du chargement du fichier\n");
        return 1;
    }

    // Calcul du spanning tree avant de lancer la simulation
    stp_calculer_spanning_tree(&reseau);
    stp_afficher_etat_ports(&reseau);

    int idx_src = 14; // station source
    int idx_dest = 7; // station destination

    simuler_trame_station(&reseau, idx_src, idx_dest);
    printf("\n=== TABLES MAC APRÈS 1ère TRAME ===\n");
    int i;
    for (i = 0; i < reseau.nb_equipements; i++) {
        if (reseau.equipements[i].type == SWITCH) {
            printf("\nSwitch %d :\n", i);
            afficher_table_mac(&reseau.equipements[i].data.sw);
        }
    }
    simuler_trame_station(&reseau, idx_dest, idx_src);
    printf("\n=== TABLES MAC APRÈS 2ème TRAME ===\n");
    for (i = 0; i < reseau.nb_equipements; i++) {
        if (reseau.equipements[i].type == SWITCH) {
            printf("\nSwitch %d :\n", i);
            afficher_table_mac(&reseau.equipements[i].data.sw);
        }
    }
    simuler_trame_station(&reseau, idx_src, idx_dest);
    return 0;
}