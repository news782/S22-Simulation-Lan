#ifndef NETWORK_CONFIG_H
#define NETWORK_CONFIG_H

#include "network_structures.h"

// Charge un réseau à partir d'un fichier de configuration
int charger_reseau(const char *filename, reseau_t *reseau);

#endif // NETWORK_CONFIG_H