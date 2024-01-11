# RFID NexusGate

## Mission 🔒
Notre mission avec le projet RFID NexusGate est de créer une porte sécurisée avec les caractéristiques suivantes :
- Base de données dynamique
- Badge RFID attribuable

## DeadLine 🟥
La date limite pour le projet est fixée au 2 février 2024. Assurez-vous de respecter les délais suivants :
- Faire un point d'avancement tous les 15 jours
- Réaliser un point intermédiaire avec Julien.V

## Objectifs
Le projet implique les étapes suivantes :
- Réaliser une présentation en groupe avec démonstration
- Produire un rapport technique détaillé
- Effectuer une évaluation 360.

## Groupe 
Les membres du groupe 1 travaillant sur le projet sont :
- Clayton
- Yannick
- Ramy
- Maxim

## Suivi du Projet
Pour suivre le progrès du projet, veuillez consulter régulièrement les mises à jour du code et les rapports dans ce dépôt GitHub.

## Contributions
Nous encourageons les contributions de la part de tous les membres du groupe. Assurez-vous de communiquer et de collaborer étroitement pour atteindre nos objectifs.

## Contact
Pour toute question ou clarification, n'hésitez pas à contacter le responsable du projet ou à utiliser la section des problèmes (Issues) sur GitHub.

## Avancé du Projet Arduino Code 
Le code contient une carte "admin" qui à sont UID déjà inscrit dans le code. Nous avonsune deuxième carte qui n'a aucun accés pour l'instant.
Lors de la lecture de la carte "admin", le messaege "Admin OK" s'aaffiche, de plus nous pouvons ajouter une "carte" en appuyant sur le bouton poussoir pour rentrer dans le mode ajout. Dans ce mode nous scannons la nouvelle carte et le message UID ajouté s'affiche et laisse place apres 3 secondes au message "Hotel Cyber". Apres cela la personne peut scanner sa carte et s'affiche donc le message "Acces OK" au lieu de "Acces refuse".

Avancé du 11 Janv : Mettre un servo moteur lors de l'affichage du mot "Acces ok" et celui ci revient a son emplcamement d'origine. Permet donc l'ouverture de notre porte.

## Composants presents
  - Arduino
  - RC522 RFID
  - GROVE LCD RGB
  - BUTTON GROVE
  - 2 Cartes RFID 

## Photos du projet 
![Photoducablage](https://cdn.discordapp.com/attachments/1195029985409388694/1195030024823242913/IMG_0753.jpg?ex=65b28170&is=65a00c70&hm=3664ac06c15cc6303e6b3a9d3047aebbba00e4d6cc857cbdae302cefdc282ff5&)
