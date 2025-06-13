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
    if (visited[courant]) return 0;
    visited[courant] = 1;

    if (courant == dest_station) {
        if (!*trouve) {
            printf("La trame arrive à la station destination (%d) !\n", courant);
            *trouve = 1;
        }
        // On continue la propagation pour simuler l'inondation complète
        return 1; // (optionnel, tu peux aussi retourner 0, mais on marque qu'on l'a trouvée)
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
            printf("Switch %d : MAC destination connue, envoie vers équipement %d\n", courant, port_equip);
            propager_trame(reseau, port_equip, courant, trame, dest_station, trouve, profondeur+1, visited);
            // Pas de return ici : on veut aussi continuer l'inondation
        } else {
            printf("Switch %d : MAC destination inconnue, inonde tous les ports\n", courant);
            int i;
            for (i = 0; i < reseau->nb_liens; i++) {
                int e1 = reseau->liens[i].equip1, e2 = reseau->liens[i].equip2;
                int voisin = (e1 == courant) ? e2 : (e2 == courant ? e1 : -1);
                if (voisin != -1 && voisin != precedent) {
                    propager_trame(reseau, voisin, courant, trame, dest_station, trouve, profondeur+1, visited);
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
    printf("\n");

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
    

    int bool = 1;

    while(bool)
    {
        printf("\n1) Afficher réseaux\n");
        printf("2) Afficher tables de commutations\n");
        printf("3) Envoyer une trame\n");
        printf("4) Afficher état STP\n");
        printf("5) Quitter\n");

        int choix; 
        printf("\nVotre choix : ");
        scanf("%d", &choix);


        if(choix == 5)
        {
            return 0;
        }

        else if(choix == 4)
        {
            stp_afficher_etat_ports(&reseau);
        }

        else if(choix == 3)
        {
            printf("\nNuméro de la station source : ");
            int stSrc;
            scanf("%d", &stSrc);

            printf("Numéro de la station destination : ");
            int stDest;
            scanf("%d", &stDest);

            simuler_trame_station(&reseau, stSrc, stDest);

        }

        else if(choix == 2)
        {
            printf("De quel switch voulez-vous voir la table ? : ");
            int choix;

            scanf("%d", &choix);

            if(reseau.equipements[choix].type == SWITCH)
            {
                printf("\n\n%d) ", choix);
                afficher_table_mac(&reseau.equipements[choix].data.sw); 
                printf("\n");
            }

            else
            {
                printf("Ce numéro d'équipement n'est pas un switch ou n'existe pas !\n");
            }
        }

        else if(choix == 1)
        {
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

            printf("\n\n");
        }

        else
        {
            printf("Veuillez selectionner un des menus !\n");
        }
    }
    
    return 0;
}