#include "network_structures.h"
#include "network_display.h"
#include "network_config.h"
#include "ethernet_frame.h"
#include "switch_logic.h"
#include <stdio.h>
#include <string.h>

// Recherche d'une station par IP
int trouver_station_par_ip(reseau_t *reseau, ip_addr_t ip) {
    for (int i = 0; i < reseau->nb_equipements; i++) {
        if (reseau->equipements[i].type == STATION &&
            memcmp(reseau->equipements[i].data.station.ip.addr, ip.addr, IP_ADDR_LEN) == 0) {
            return i;
        }
    }
    return -1;
}

// Fonction récursive : propagation de la trame
int propager_trame(reseau_t *reseau, int courant, int precedent, 
                   const ethernet_frame_t *trame, int dest_station, int *trouve, int profondeur) {
    // Anti-boucle : profondeur max
    if (profondeur > reseau->nb_equipements) return 0;

    if (courant == dest_station) {
        printf("La trame arrive à la station destination (%d) !\n", courant);
        *trouve = 1;
        return 1;
    }

    equipement_t *eq = &reseau->equipements[courant];
    if (eq->type == SWITCH) {
        switch_t *sw = &eq->data.sw;
        // Apprentissage MAC source
        switch_apprendre_mac(sw, trame->src, precedent);

        // Affichage table MAC (optionnel, pour debug)
        //afficher_table_mac(sw);

        int port = switch_rechercher_port(sw, trame->dest);
        if (port != -1 && port != precedent) {
            // On connait le port de sortie
            printf("Switch %d : MAC destination connue, envoie sur port %d\n", courant, port);
            // Chercher l'équipement voisin correspondant à ce port
            for (int i = 0; i < reseau->nb_liens; i++) {
                int e1 = reseau->liens[i].equip1, e2 = reseau->liens[i].equip2;
                int voisin = (e1 == courant) ? e2 : (e2 == courant ? e1 : -1);
                if (voisin != -1 && voisin != precedent) {
                    if (propager_trame(reseau, voisin, courant, trame, dest_station, trouve, profondeur+1)) {
                        return 1;
                    }
                }
            }
        } else {
            // Inonde sur tous les ports sauf celui d'arrivée
            printf("Switch %d : MAC destination inconnue, inonde tous les ports\n", courant);
            for (int i = 0; i < reseau->nb_liens; i++) {
                int e1 = reseau->liens[i].equip1, e2 = reseau->liens[i].equip2;
                int voisin = (e1 == courant) ? e2 : (e2 == courant ? e1 : -1);
                if (voisin != -1 && voisin != precedent) {
                    if (propager_trame(reseau, voisin, courant, trame, dest_station, trouve, profondeur+1)) {
                        return 1;
                    }
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

    // On propage la trame à tous les voisins (liens) de la station source
    int trouve = 0;
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

    // Simulation trame
    int idx_src = 14; // exemple : station 14
    int idx_dest = 7; // exemple : station 7
    simuler_trame_station(&reseau, idx_src, idx_dest);

    // Tu peux tester en envoyant une autre trame ensuite
    simuler_trame_station(&reseau, idx_src, idx_dest);


    return 0;
}