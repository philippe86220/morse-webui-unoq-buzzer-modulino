# Morse WebUI – Arduino UNO Q

Ce projet propose une **interface Web complète** permettant de jouer du **code Morse** sur une **Arduino UNO Q**,
en s’appuyant sur l’architecture spécifique UNO Q (Linux + MCU) et sur le **Bridge Arduino**.

Le rendu sonore est assuré par un **Modulino Buzzer**, avec un **timing Morse réaliste** et une **vitesse réglable**.
L’interface Web fournit également une **représentation visuelle pédagogique** des points et des traits.

Ce projet est à la fois :
- un démonstrateur technique de l’UNO Q,
- un outil pédagogique autour du Morse,
- un exemple clair d’intégration **Web → Python → MCU**.

---

## Fonctionnalités

### Interface Web
- Saisie libre d’un texte à transmettre en Morse
- Réglage de la vitesse (curseur, de 5 à 30)
- Mise à jour dynamique de la valeur de vitesse
- Représentation visuelle du Morse (points et traits)
- Interface lisible, sobre et adaptée à un usage pédagogique
- Utilisation possible depuis plusieurs onglets du navigateur

### Côté Python (UNO Q – Linux)
- API HTTP simple via WebUI
- Mise en file d’attente des requêtes Morse
- Gestion d’un état occupé / libre (évite les collisions)
- Synchronisation par verrou (thread-safe)
- Communication fiable avec le MCU via Bridge

### Côté MCU (Arduino)
- Réception du texte et de la vitesse
- Décodage Morse caractère par caractère
- Respect des timings Morse standards :
  - point = 1 unité
  - trait = 3 unités
  - espace inter-élément = 1 unité
  - espace inter-lettre = 3 unités
  - espace inter-mot ≈ 7–8 unités
- Génération sonore via Modulino Buzzer

---

## Aperçu

![Morse WebUI screenshot](screenshots/ui.png)

---

## Architecture générale

```
Navigateur Web
   ↓ HTTP (WebUI)
main.py (Linux – UNO Q)
   ↓ Bridge.call("morse_play")
MCU Arduino UNO Q
   ↓
Modulino Buzzer
```

Cette séparation claire permet :
- une interface Web indépendante,
- une logique de synchronisation robuste côté Python,
- un code temps réel simple et efficace côté MCU.

---

## Organisation du dépôt

```
├- morse-webui-uno-q-buzzer-modulino/
├── README.md
├── LICENSE
├── assets/
│   ├── index.html
│   └── app.js
├── python/
│   └── main.py
├── sketch/
│   ├── sketch.ino
│   ├── Morse.h
│   └── Morse.cpp
└── screenshots/
    └── ui.png
```

### Description des composants

#### `assets/`
- **index.html** : structure HTML et styles CSS
- **app.js** : logique JavaScript, appels API, rendu visuel du Morse

#### `python/main.py`
- Exposition des endpoints HTTP (`/morse`, `/status`, `/speed`, `/ping`)
- Gestion d’une file d’attente des requêtes
- Appels Bridge vers le MCU
- Protection des ressources partagées par verrou

#### `sketch/`
- **sketch.ino** : point d’entrée Arduino, initialisation Bridge et buzzer
- **Morse.h / Morse.cpp** : implémentation complète du Morse (table, timings, lecture)

---

## API HTTP

### Ping
```
GET /ping
```
Vérifie que l’API est accessible.

### État
```
GET /status
```
Retourne :
- état occupé ou libre
- longueur de la file d’attente
- dernière action
- vitesse actuelle

### Vitesse
```
GET /speed
GET /speed?value=17
```
Lecture ou modification de la vitesse globale.

### Lecture Morse
```
GET /morse?data=bonjour&speed=17
```
Ajoute un texte à la file d’attente pour lecture en Morse.

---

## Tables Morse

- Une table Morse est définie côté **Web** (JavaScript) pour l’affichage graphique.
- Une table équivalente est définie côté **MCU** pour la génération sonore.

Cette duplication est volontaire :
- le Web se concentre sur l’affichage pédagogique,
- le MCU se concentre sur le temps réel et le son.

---

## Notes techniques

- Les caractères non supportés sont ignorés côté MCU
- Le texte est copié dans un buffer C statique (sécurité mémoire)
- La vitesse est calculée à partir d’une base WPM standard
- Plusieurs requêtes peuvent être envoyées sans bloquer l’interface
- Le projet a été validé à l’oreille par un ancien instructeur Morse

---

## Objectifs du projet

- Explorer les possibilités de l’architecture Arduino UNO Q
- Mettre en pratique une communication Web → Python → MCU
- Proposer un outil Morse simple, réaliste et pédagogique
- Servir de base à d’autres projets (LED, animation, apprentissage du Morse)

---

## Licence

Ce projet est diffusé sous licence **MIT**.

---

## Auteur

Projet personnel autour du code Morse, de l’Arduino UNO Q et des interfaces Web.

## Remerciements

Ce projet a été conçu et structuré avec l’aide de ChatGPT (OpenAI),
utilisé comme assistant technique pour la réflexion,
la clarification de l’architecture et la mise en forme du projet.
