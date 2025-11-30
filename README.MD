# 📂 SAE Réseaux – Simulation de Commutation Ethernet & Spanning Tree Protocol

## 🌟 Aperçu du Projet
**Objectif :**

Développer une application en C simulant le fonctionnement d’un réseau Ethernet, incluant la commutation, l’apprentissage MAC et la prévention des boucles via le protocole Spanning Tree Protocol (STP).

### 🔌 Lecture de Topologie (`.lan`)
- Chargement des stations, switches et liens depuis un fichier texte.
- Interprétation automatique de la structure réseau.

### 🗺️ Affichage de la Topologie
- Visualisation des équipements du réseau.
- Affichage des interconnexions entre les éléments.

### 📡 Simulation de Trames Ethernet
- Création de trames réalistes (preambule, adresses MAC, type, données, FCS).
- Circulation complète d'une trame de bout en bout.

### 🔁 Switch avec Table MAC Dynamique
- Apprentissage automatique des adresses MAC.
- Commutation ciblée vers le port approprié.
- Inondation lorsque la MAC de destination est inconnue.

### 🌳 Implémentation du Spanning Tree Protocol (STP)
- Élection du switch root.
- Gestion des ports : actifs / bloqués.
- Suppression automatique des boucles réseau.

### 📊 Affichage Avancé
- État détaillé des ports de chaque switch.
- Affichage du root du Spanning Tree.
- Visualisation du parcours de la trame dans le réseau.

---

## 🛠️ Technologies

- **Type :** Application console en C  
- **Langage :** C (GCC)  
- **Compilation :** Makefile inclus  
- **Plateforme :** Linux / WSL / macOS  

## 🚀 Compilation
Pour exécuter la simulation sur votre machine :

**Prérequis**
-gcc installé
-make installé

```sh
make
```

Pour nettoyer :
```sh
make clean
```

**Lancer la simulation**

```sh
./simulateur_reseau <nom_du_fichier.lan>
```

Exemple :
```sh
./simulateur_reseau mylan.lan
```

## 📄 Structure d’un fichier `.lan`

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

## 📄 Fichiers principaux

- `main.c` : point d’entrée, gestion de la simulation
- `network_structures.c/h` : structures de données réseau
- `network_display.c/h` : affichage
- `network_config.c/h` : lecture des fichiers .lan
- `ethernet_frame.c/h` : gestion des trames Ethernet
- `switch_logic.c/h` : logique de commutation
- `stp.c/h` : algorithme spanning tree


---

##🧑‍💻 Auteurs

Klausnitzer Jules
📧 Contact: julot424@gmail.com

Caubert Martin
📧 Contact: cartinmaubert@gmail.com

---

