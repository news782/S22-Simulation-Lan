#include "stp.h"
#include <stdio.h>

// Structure simple pour retenir pour chaque switch : distance au root et port root
typedef struct {
    int distance;
    int port_root; // voisin par lequel passer pour joindre le root
} stp_info_t;

// Trouve l'index du root : plus petite priorité, puis plus petite MAC
int stp_trouver_root(reseau_t *reseau) {
    int idx_root = -1;
    int i, j;
    for (i = 0; i < reseau->nb_equipements; i++) {
        if (reseau->equipements[i].type == SWITCH) {
            if (idx_root == -1) {
                idx_root = i;
            } else {
                switch_t *sw = &reseau->equipements[i].data.sw;
                switch_t *sw_root = &reseau->equipements[idx_root].data.sw;
                if (sw->priority < sw_root->priority) {
                    idx_root = i;
                } else if (sw->priority == sw_root->priority) {
                    for (j = 0; j < MAC_ADDR_LEN; j++) {
                        if (sw->mac.addr[j] < sw_root->mac.addr[j]) {
                            idx_root = i;
                            break;
                        } else if (sw->mac.addr[j] > sw_root->mac.addr[j]) {
                            break;
                        }
                    }
                }
            }
        }
    }
    return idx_root;
}

// BFS pour calculer la distance au root et mémoriser le port root pour chaque switch
void stp_calculer_spanning_tree(reseau_t *reseau) {
    int i, j;
    int root = stp_trouver_root(reseau);

    // Initialisation des infos
    stp_info_t stp_infos[MAX_SWITCHES + MAX_STATIONS];
    for (i = 0; i < reseau->nb_equipements; i++) {
        stp_infos[i].distance = 10000; // "infini"
        stp_infos[i].port_root = -1;
    }

    stp_infos[root].distance = 0;

    // On fait un BFS "à la main"
    int file[MAX_SWITCHES + MAX_STATIONS];
    int debut = 0, fin = 0;
    file[fin++] = root;

    while (debut < fin) {
        int courant = file[debut++];
        for (i = 0; i < reseau->nb_liens; i++) {
            int voisin = -1;
            if (reseau->liens[i].equip1 == courant)
                voisin = reseau->liens[i].equip2;
            else if (reseau->liens[i].equip2 == courant)
                voisin = reseau->liens[i].equip1;

            if (voisin != -1 && reseau->equipements[voisin].type == SWITCH) {
                if (stp_infos[voisin].distance > stp_infos[courant].distance + 1) {
                    stp_infos[voisin].distance = stp_infos[courant].distance + 1;
                    stp_infos[voisin].port_root = courant;
                    file[fin++] = voisin;
                }
            }
        }
    }

    // Initialisation des états de port : tout bloqué par défaut
    for (i = 0; i < reseau->nb_equipements; i++) {
        if (reseau->equipements[i].type == SWITCH) {
            switch_t *sw = &reseau->equipements[i].data.sw;
            for (j = 0; j < MAX_PORTS; j++) sw->port_etat[j] = 0;
        }
    }

    // Pour chaque switch, on active le port de son root
    for (i = 0; i < reseau->nb_equipements; i++) {
        if (i == root) continue;
        if (reseau->equipements[i].type == SWITCH && stp_infos[i].port_root != -1) {
            // On cherche sur quel port (numéro d'équipement voisin) on a le root
            switch_t *sw = &reseau->equipements[i].data.sw;
            // On active le port menant au root
            for (j = 0; j < MAX_PORTS; j++) {
                if (sw->port_table[j] == stp_infos[i].port_root) {
                    sw->port_etat[j] = 1; // 1 = actif (dans le spanning tree)
                }
            }
        }
    }

    // On active les ports désignés (vers les voisins pour lesquels on est le plus proche du root)
    for (i = 0; i < reseau->nb_liens; i++) {
        int a = reseau->liens[i].equip1;
        int b = reseau->liens[i].equip2;
        if (reseau->equipements[a].type != SWITCH || reseau->equipements[b].type != SWITCH) continue;

        int da = stp_infos[a].distance;
        int db = stp_infos[b].distance;

        if (da < db) {
            // a est plus proche du root, il est "designated bridge" pour ce lien
            switch_t *sw = &reseau->equipements[a].data.sw;
            // active le port vers b
            for (j = 0; j < MAX_PORTS; j++) {
                if (sw->port_table[j] == b) sw->port_etat[j] = 1;
            }
        } else if (db < da) {
            // b est designated bridge pour ce lien
            switch_t *sw = &reseau->equipements[b].data.sw;
            for (j = 0; j < MAX_PORTS; j++) {
                if (sw->port_table[j] == a) sw->port_etat[j] = 1;
            }
        } else if (da == db && da != 10000) {
            // Egalité, on prend le switch avec la plus petite MAC/prio
            switch_t *swa = &reseau->equipements[a].data.sw;
            switch_t *swb = &reseau->equipements[b].data.sw;
            int swa_avant = 0;
            if (swa->priority < swb->priority) swa_avant = 1;
            else if (swa->priority == swb->priority) {
                for (j = 0; j < MAC_ADDR_LEN; j++) {
                    if (swa->mac.addr[j] < swb->mac.addr[j]) { swa_avant = 1; break; }
                    else if (swa->mac.addr[j] > swb->mac.addr[j]) { swa_avant = 0; break; }
                }
            }
            if (swa_avant) {
                for (j = 0; j < MAX_PORTS; j++) {
                    if (swa->port_table[j] == b) swa->port_etat[j] = 1;
                }
            } else {
                for (j = 0; j < MAX_PORTS; j++) {
                    if (swb->port_table[j] == a) swb->port_etat[j] = 1;
                }
            }
        }
    }
}

// Affiche l'état des ports pour chaque switch
void stp_afficher_etat_ports(reseau_t *reseau) {
    int i, j;
    printf("\n=== État des ports (1 = actif, 0 = bloqué) ===\n");
    for (i = 0; i < reseau->nb_equipements; i++) {
        if (reseau->equipements[i].type == SWITCH) {
            printf("Switch %d :\n", i);
            switch_t *sw = &reseau->equipements[i].data.sw;
            for (j = 0; j < sw->nb_ports; j++) {
                printf("  Port vers %d : %d\n", sw->port_table[j], sw->port_etat[j]);
            }
        }
    }
}