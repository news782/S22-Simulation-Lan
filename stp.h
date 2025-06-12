#ifndef STP_H
#define STP_H

#include "network_structures.h"

void stp_calculer_spanning_tree(reseau_t *reseau);
void stp_afficher_etat_ports(reseau_t *reseau);

#endif