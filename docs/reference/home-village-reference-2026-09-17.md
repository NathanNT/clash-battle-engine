# Référence Home Village figée — 2026-09-17

## Portée

Cette référence couvre le combat d'attaque du **Home Village** : troupes,
sorts, bâtiments destructibles, défenses, murs et pièges. Les héros, familiers,
engins de siège et équipements restent inscrits comme extensions de combat
obligatoires à la matrice afin de ne pas les perdre de vue lorsque leur interface
est ajoutée au mode RL.

Builder Base, Clan Capital, événements temporaires et contenu purement cosmétique
sont hors périmètre.

## Date et preuves de référence

Le point de référence est fixé au **17 septembre 2026, Europe/Paris**. La dernière
mise à jour permanente identifiée avant ce point est l'« August Update » du
30 août 2026. Les entrées d'archive du 1er et du 9 septembre sont des événements,
pas une mise à jour permanente du catalogue Home Village.

Sources primaires :

- Supercell, [August Update, 30 Aug 2026](https://supercell.com/en/games/clashofclans/blog/release-notes/august-update-3/) ; elle confirme notamment les niveaux TH18 de Diggy et les supercharges de Builder's Hut et Monolith.
- Supercell, [archive Clash of Clans](https://supercell.com/en/games/clashofclans/blog/), consultée le 17 septembre 2026 ; elle place bien l'August Update avant les événements de septembre.

La source de données structurées actuellement figée dans le dépôt est
`chiefpansancolt/clash-of-clans-data@62b019df868f9decb49e97a0ff5ae15fb27af9ba`
(19 avril 2026). Son dernier commit Home Village public est également daté du
19 avril 2026. Elle est utile pour les schémas et valeurs historiques, mais **ne
constitue pas à elle seule une preuve suffisante de fidélité à la référence de
septembre**. Toute ligne reposant seulement sur elle reste `a_verifier` dans la
matrice.

Le complément versionné [TH18 2026-09-17](../../data/reference/th18-2026-09-17.json)
est le registre de rapprochement : il conserve les notes Supercell consultées,
leurs dates, les deltas officiellement publiés et les overrides champ par champ.
Il est volontairement marqué `provisional_official_delta_inventory` : Supercell
ne publie pas une table numérique exhaustive. Une valeur absente du registre ne
peut pas être inventée ni promue en donnée vérifiée.

## Règle de clôture

Une ligne ne passe à `verifiee` que lorsque : données datées et traçables,
comportement moteur, interface Viewer/RL, replay déterministe et test de
régression sont tous présents. Une approximation ou une absence de source garde
la ligne ouverte.
