# SAE Réseaux – Simulation de Commutation Ethernet & Spanning Tree Protocol

## Objectif

Ce projet simule un réseau local Ethernet comprenant :
- La lecture d’une topologie de réseau à partir d’un fichier `.lan`
- La création et l’affichage de trames Ethernet réalistes
- La commutation de trames par les switches avec table d’apprentissage MAC
- La gestion automatique des boucles réseau avec le protocole Spanning Tree Protocol (STP)

## Fonctionnalités principales

- Chargement dynamique d’un réseau (stations, switches, liens) depuis un fichier texte
- Affichage de la topologie, des équipements et des liens
- Simulation de la circulation d’une trame Ethernet dans le réseau
- Table de commutation MAC dynamique pour chaque switch (apprentissage automatique)
- Détection et suppression automatique des boucles via un algorithme STP (élection du root, ports bloqués/désignés)
- Affichage de l’état des ports de chaque switch (actif/bloqué) et du root du spanning tree

## Compilation

Assurez-vous d’avoir `gcc` installé.

```sh
make
```

Pour nettoyer :
```sh
make clean
```

## Lancer la simulation

```sh
./simulateur_reseau <nom_du_fichier.lan>
```

Exemple :
```sh
./simulateur_reseau mylan.lan
```

## Structure d’un fichier `.lan`

- La première ligne indique le nombre d’équipements et de liens
- Les lignes suivantes décrivent chaque équipement (switch ou station)
- Enfin, chaque lien (câble) est décrit par deux indices d’équipement et un poids

Exemple :
```
15 11
2;01:45:23:a6:f7:01;8;1024
...
0;1;4
...
```

## Auteurs

CAUBERT Martin - KLAUSNITZER Jules

---

## Consignes de la SAE

- Respecter la structure et le fonctionnement des trames Ethernet (preambule, adresses MAC, type, données, FCS)
- Implémenter un switch “intelligent” avec apprentissage MAC (commutation + inondation si MAC inconnue)
- Simuler le fonctionnement du Spanning Tree Protocol pour garantir l’absence de boucle dans le réseau
- Afficher clairement :
    - Les états des ports de chaque switch (bloqué/actif)
    - Le root du spanning tree
    - La circulation des trames dans le réseau

---

## Bonus possibles

- Simulation plus réaliste du STP (BPDU, convergence progressive…)
- Prise en compte des poids de liens (STP pondéré)
- Interface graphique ou visualisation

---

## Fichiers principaux

- `main.c` : point d’entrée, gestion de la simulation
- `network_structures.c/h` : structures de données réseau
- `network_display.c/h` : affichage
- `network_config.c/h` : lecture des fichiers .lan
- `ethernet_frame.c/h` : gestion des trames Ethernet
- `switch_logic.c/h` : logique de commutation
- `stp.c/h` : algorithme spanning tree

---