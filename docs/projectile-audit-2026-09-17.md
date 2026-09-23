# Audit des projectiles exécutables — référence TH18

Date d'audit : 2026-09-17. Périmètre : seuls les éléments actuellement
exécutables par `cocsim_core`; les entrées catalogue non jouables ne sont pas
présentées comme des armes simulées. Le catalogue gelé contient 59 candidats à
portée longue hors Guérisseuse : 22 suivent aujourd'hui le pipeline d'arme
exécutable, tandis que 37 restent données seulement. La Guérisseuse et la Mine
aérienne chercheuse sont les deux chemins spéciaux exécutables et auditables
séparément. Ces frontières chiffrées sont contrôlées par le validateur, plutôt
que déduites d'un statut de rendu ou d'un placeholder.

La matrice machine-lisible par élément est
[`projectile-contracts-2026-09-17.json`](../data/reference/projectile-contracts-2026-09-17.json).
L’inventaire complémentaire de tous les chemins qui peuvent modifier les PV est
[`combat-damage-paths-2026-09-17.json`](../data/reference/combat-damage-paths-2026-09-17.json).

Identifiants catalogue audités : `archer`, `cannon`, `archer_tower`, `mortar`,
`wizard`, `dragon`, `eagle_artillery`, `inferno_tower`, `minion`,
`air_defense`, `wizard_tower`, `x_bow`, `dragon_rider`, `baby_dragon`,
`bomb_tower`, `hidden_tesla`, `multi_archer_tower`, `multi_gear_tower`, `scattershot`, `thrower`, `electro_titan`,
`healer`, `seeking_air_mine`.

## Contrat actuel du Core

Un tir marqué `ranged` crée un `BattleState::Projectile`, qui est sérialisé,
restauré, inclus dans le hash et projeté en lecture seule vers le GUI et RL,
sauf lorsqu’un contrat d’impact immédiat est explicitement sourcé. La Tour de
l’enfer est actuellement cette exception : elle applique un rayon direct
déterministe, sans projectile sérialisé.
Sans vitesse publique, il conserve le contrat historique d'impact au prochain
tick (`T+10 ms`). Ce délai ne prétend pas reproduire le client et ne doit pas
être interprété comme une vitesse. Une arme dont le catalogue contient une
vitesse progresse au contraire de façon homing, à chaque `kTickMs`, jusqu'à sa
cible. La Guérisseuse suit aussi ce pipeline avec un projectile logique de soin
car aucune source gelée ne documente un impact immédiat.

| Élément exécutable | Nature visuelle/jeu connue publiquement | Core au 2026-09-17 | Vitesse numérique exploitable | Fidélité de trajet |
| --- | --- | --- | --- | --- |
| Archer, Tour d’archers, Canon, Mortier, Tour d’archers multiple, Canon à engrenages multiple | attaque à distance ; les tables épinglées ne publient pas de vitesse | Projectile logique `T+10 ms` | non publiée | ouverte |
| Catapulte | cône arrière à 90° : retombée à 1 puis 5 cases et filtrage air/sol issus d'une source secondaire ; munitions et vol exact non publiés | Projectile logique `T+10 ms`, suivi du cône déterministe à l’impact | non publiée | partielle |
| Sorcier, Dragon, Minion, Bébé dragon, Dragon Rider | attaque à distance ; pas de vitesse dans les sources gelées | Projectile logique `T+10 ms` | non publiée | ouverte |
| Lanceur | tir mono-cible à longue portée ; la table actuelle donne niveaux, portée et cadence, pas le vol au tick | Projectile logique `T+10 ms` | non publiée | ouverte |
| Électro-titan | coup de fouet à distance ; aucune vitesse de projectile versionnée | Projectile logique `T+10 ms` | non publiée | ouverte ; l’aura est auditée séparément |
| Aigle artillerie, Défense aérienne, Tour de sorciers, X-Bow, Tour à bombes | attaque à distance ; les sources portent cadence/rayon/cible, pas le vol au tick. L’Aigle conserve sa rafale source 3 × 0,75 s puis pause 10 s. | Projectile logique `T+10 ms` | non publiée | ouverte |
| Hôtel de ville armé (`town_hall`, TH12–TH17) | Giga Tesla/Giga Inferno puis Artillerie infernale : 4 cibles simultanées dans le catalogue. La note officielle TH17 confirme les 4 projectiles et une zone de dégâts persistante. | 4 projectiles logiques `T+10 ms` | non publiée | partielle : zone persistante TH17 non modélisée |
| Tour de l’enfer | rayon mono ou cinq rayons, confirmé par source secondaire datée | dégât direct au tick, sans projectile | sans objet (rayon) | partielle : ordre au tick/re-ciblage ouverts |
| Tesla camouflée | arme à distance ; les sources portent cadence/cible, pas le vol au tick | Projectile logique `T+10 ms` | non publiée | ouverte |
| Guérisseuse | soin longue portée ; contrat direct/vol non publié | Projectile logique de soin `T+10 ms` | non publiée | ouverte |
| Mine aérienne chercheuse | projectile chercheur | projectile homing Core | **3,5 cases/s** | partielle : vitesse seulement |

La [note Supercell du TH17](https://supercell.com/en/games/clashofclans/blog/game-updates/the-town-hall-17-update-is-here-2/), publiée le 25 novembre 2024 et consultée le 17 septembre 2026, confirme la salve de quatre projectiles de l'Artillerie infernale et sa zone persistante. Le snapshot secondaire épinglé fournit `flameMaxDps=75` et `flameDuration=6,8 s` pour TH17, mais ni son rayon, ni ses impulsions, ni l’ordre de perte de cible ; ces paramètres ne sont donc pas appliqués. Le même snapshot ne contient aucune arme imbriquée au TH18 : aucun comportement armé TH18 n’est attribué sans preuve.

## Audit des chemins de dégâts non-projectile

Une recherche par portée seule manquerait les sorts, pièges et effets post-mort.
 Le registre `combat-damage-paths` fixe donc les onze familles de contrôle qui
appellent `damage` ou modifient des PV : projectile d’arme, rayon Inferno,
contact normal/splash, contact Mur du Root Rider, sorts, soin de sort,
explosion post-mort, aura périodique, piège déclenché et Tornade. Chaque famille indique l’état
futur sérialisé et sa projection GUI/RL.

La règle vérifiée est stricte : dans `update_entities`, toute arme `ranged`
entre dans `projectiles_`, sauf le drapeau explicite `inferno_beam`. Les autres
chemins directs sont des contacts, casts, déclenchements ou effets de zone ; ils
ne sont jamais déguisés en munition pour le rendu. Le validateur
`projectile_audit_validation` contrôle à la fois la liste complète des familles
et ce garde-fou dans le Core. Cela ne rend pas leurs timings fidèles par défaut :
leurs incertitudes restent dans le delta TH18.

L’Électro-titan apporte le chemin `passive_aura` : toutes les 400 ms après son
déploiement, le Core applique les dégâts d’aura importés au niveau concerné à
toute entité ennemie visible, sol ou air, dans les 3,5 cases, mais jamais à un
Mur. Cette impulsion ne dépend ni de Rage ni de Gel. Son prochain tick est
dans l’état logique sérialisé ; le GUI et RL ne reçoivent que les événements
résolus. La source ne fixe pas la première impulsion ni les interactions avec
les cibles cachées ou souterraines : cette route reste donc partielle.

## Preuve numérique utilisable

La note officielle Supercell du 11 août 2021 indique que la vitesse du
projectile de la Mine aérienne chercheuse est passée de 2,5 à 3,5 cases/s :
<https://supercell.com/en/games/clashofclans/blog/release-notes/balance-changes-august21/>.
Cette valeur est une surcharge versionnée dans
`data/reference/th18-2026-09-17.json`, transmise à
`projectile_speed_tiles_per_second` avec sa provenance.

La même note ne donne ni délai de lancement, ni rayon de collision, ni ordre
d'annulation lorsque la cible est détruite. Le moteur utilise donc une
trajectoire homing discrète reproductible, mais ces points restent ouverts dans
`th18-documentation-delta-2026-09-17.json`.

Le drapeau `homing` est une règle Core sérialisée, et non une propriété de
rendu. Les armes à distance ordinaires ne lisent donc pas une vitesse numérique
isolée pour devenir chercheuses : leur projectile logique reste à `T+10 ms`
tant qu'un contrat versionné n'a pas déterminé le lancement, l'extrémité et la
perte de cible. Cette barrière empêche un futur import de données de modifier
silencieusement les résultats headless, replay ou RL.

La note officielle Supercell du 27 avril 2026 apporte un autre timing
directement exploitable : la première rafale du Mortier amélioré attend 0,5 s
au lieu de partir immédiatement. La valeur est importée dans le catalogue
immuable (`first_burst_delay_seconds`) et mémorisée par le `next_action`
sérialisé de l’entité. Elle est aussi exposée telle quelle, en millisecondes
quantifiées, par `catalogue_stats()[...]["first_burst_delay_ms"]` : RL lit donc
la même donnée immuable que le Core, sans temporisateur parallèle. Elle ne
fournit en revanche aucune vitesse de coque.

Les champs RL de catalogue exposent également, pour chaque niveau, la
disponibilité et les paramètres importés du mode `geared_up_burst` (dégât,
cadence, nombre de tirs, pause et portée). Une requête RL peut ainsi proposer
ce mode sans recopier ses chiffres ; l'instanciation reste validée par le Core
et rejette les niveaux sans donnée complète.

La nature « rayon » de la Tour de l’enfer est documentée séparément par la
source secondaire consultée le 17 septembre 2026 :
<https://www.clash.ninja/guides/the-inferno-tower>. Elle décrit un rayon ciblé
unique ou cinq rayons plus petits. Elle justifie l’absence de délai de vol, mais
ne publie pas l’ordre de résolution exact au tick, la fenêtre de re-ciblage ni
un build client : sa fidélité reste donc partielle.

## Projection sans influence sur le combat

`ProjectileView` contient un identifiant stable, l’origine, la position au
tick, la position courante de la cible, la vitesse, l’échéance d’impact si elle
est déjà déterminée et l’état homing. Une échéance à zéro signifie qu’un vol
homing ne peut pas la prédire. Le GUI dessine le segment et l’orbe à partir de
cette projection ; l’observation RL standard
(`NativeBattle.observation()["projectiles"]`) expose ces mêmes champs, dont
`target_x`/`target_y` et `scheduled_impact_ms`, sans recalculer une cible côté
Python. La régression RL lit à la fois le zéro du homing de la Mine et
l’échéance `T+20 ms` d’un tir logique de Canon, puis vérifie sa disparition
après l’impact.
Le catalogue RL expose aussi `projectile_speed_sourced` : ce booléen provient
de la présence d'une valeur numérique dans la référence figée, pas d'un test
sur la vitesse elle-même. Une vitesse par défaut future ne peut donc pas être
présentée comme une provenance vérifiée.
Chaque `ProjectileView` actif propage ce fait sous `speed_sourced`, que le
Viewer utilise pour sélectionner le cyan sans inférer une règle de homing.
`NativeBattle.projectile_views()` est le raccourci équivalent pour les clients
qui ne souhaitent lire que cette collection. L’identifiant est sérialisé avec le compteur de
prochain projectile dans Snapshot V14, ce qui évite de confondre deux tirs du
même auteur vers la même cible. Aucun adaptateur
ne modifie `BattleState`, et les tests C++, headless/GUI et RL couvrent les
snapshots et la trajectoire de la Mine aérienne chercheuse. Le test C++ couvre
aussi un round-trip de replay : les deux commandes de déploiement sont
sérialisées, relues, puis doivent recréer le même projectile en vol et le même
hash que le snapshot à T+20 ms. Une seconde exécution lit toutes les vues, les
projectiles et le journal d'événements à chaque tick pendant le trajet et
jusqu'à l'impact ; son hash et sa séquence d'événements restent identiques à
l'exécution headless non observée.
Chaque événement Core de lancement et d'impact de projectile — y compris le
projectile logique de soin de la Guérisseuse — porte aussi cet identifiant stable
(`projectile_id` dans `events_since`). Le Viewer associe ainsi
l'impact cyan de la Mine à son propre lancement, même lorsque le projectile a
été retiré de la collection active entre deux images. Cette association est un
cache de présentation local : la copie d'identité portée par l'événement ne
modifie ni le hash, ni le snapshot, ni le replay, ni une décision de combat.
Le soin appliqué par la Guérisseuse conserve également ce même identifiant,
afin que les consommateurs RL puissent corréler lancement, effet et impact
sans réimplémenter un suivi de cible.
Les impacts directs documentés emploient le même flux de lecture :
`NativeBattle.events_since(cursor)` retourne les événements Core ordonnés, leurs
temps `T+ms`, acteurs, cibles, coordonnées d'origine/cible capturées au tick et
un curseur suivant. La Tour de l’enfer y émet `attack` / `inferno beam`; cette
lecture ne modifie pas le hash, les commandes, le replay ni le snapshot.
Une régression supplémentaire lance simultanément deux Mines aériennes
chercheuses : leurs identifiants distincts (`1`, `2`) survivent au
snapshot/restauration avec un hash identique.
Un Snapshot V14 dont le compteur de prochain projectile réutiliserait un ID en
vol est refusé explicitement ; il n'est jamais « réparé » silencieusement en
un état qui divergerait pour GUI, RL ou replay.

Le lecteur de snapshots accepte encore V2 à V13. Pour ces formats, qui ne
portaient pas d'identifiant de projectile, les identifiants sont attribués dans
l'ordre sérialisé à la restauration ; Snapshot V13 est requis pour préserver
ces identités à l'octet près.

Les tirs à un tick sont plus courts qu’une image à 30 FPS. Pour qu’ils restent
lisibles sans leur attribuer de vitesse fictive, le GUI rejoue donc après leur
impact Core une **trajectoire cosmétique de 140 ms** (orbe + trace ambre), puis
garde brièvement le point d’impact jusqu’à 180 ms. Cette horloge de présentation
ne crée aucun projectile Core et ne peut modifier ni dégât, ni re-ciblage, ni
snapshot, replay ou hash. Le même cache utilise le **vert** pour le projectile
de soin de la Guérisseuse, afin de ne pas le faire passer pour un dégât. Les projectiles mobiles réels restent
**cyan** : le GUI dessine leur segment complet jusqu'à la position courante de
la cible portée par `ProjectileView`, même si son sprite est omis par le budget
de rendu, puis place l'orbe à la position courante calculée par Core.
Le rayon direct de la Tour de l’enfer est conservé 100 ms dans un **tracé
violet**, sans orbe ni entité projectile : ses deux extrémités viennent de
l'événement Core au tick de tir, y compris si la cible disparaît avant la frame
suivante. Ce cache GUI est lui aussi local et ne peut modifier l’impact déjà
résolu par Core.
La régression Canon/Barbare vérifie séparément qu'un tir sans vitesse publiée
reste un projectile logique en vol pendant un tick, sans dégâts anticipés, et
qu'il survit à une restauration de snapshot.
Une régression Mine aérienne/Dragon en mouvement vérifie que l'extrémité du
segment suit la position cible à chaque tick et conserve exactement cette
projection après snapshot/restauration ; aucune interpolation SDL ne participe
au calcul.
Une seconde régression détruit cette cible pendant le vol, puis sérialise la
Mine encore active. À la reprise, puis après export/import du replay, elle
produit exactement un impact terminal sans dégât et disparaît avec le même hash
sur les trois exécutions. C’est une politique déterministe documentée, non une
affirmation sur l’ordre client non publié de perte de cible.

Une régression concurrente lance dix projectiles logiques de Canon contre un
unique Barbare. Les neuf premiers dégâts le détruisent ; les dix impacts sont
néanmoins observés dans leur ordre stable d'identifiant et le dernier ne
recible ni ne crée de dégât fantôme. Cette règle de résolution protège le
déterminisme, le hash et la restauration ; elle n'est pas une affirmation de
fidélité tant que Supercell ne publie pas l'ordre de perte de cible en vol.

Le test Core exécute aussi le chemin de lancement de chacune des 16 armes à
distance actuellement jouables : Archer, Sorcier, Dragon, Serviteur, Bébé
dragon, Dragon Rider, Canon, Tour d'archers, Mortier,
Défense aérienne, Tour de sorciers, X-Bow, Tour à bombes, Tesla camouflée et
Aigle artillerie, puis Hôtel de ville armé. L'Aigle reçoit sept Golems (210 logements) pour franchir son
seuil source de 200 ; les autres scénarios vérifient qu'un `EventType::Projectile`
est émis par l'auteur attendu. Le Town Hall dispose aussi de sa régression
propre : quatre Barbares provoquent quatre projectiles d'identifiants stables,
dont l'état en vol et les impacts survivent à une restauration. La Tour de l’enfer est vérifiée séparément :
elle émet `inferno beam`, applique ses dégâts sans `EventType::Projectile` et
conserve son état de rampe par snapshot. La Mine aérienne chercheuse et la branche de
soin de la Guérisseuse ont leurs propres régressions d'état et d'impact. La
régression de soin vérifie notamment l'absence de soin au tick de lancement,
la présence du projectile logique, puis l'impact après snapshot/restauration.
Le Canon couvre aussi les deux voies de persistance d'un projectile logique :
un replay depuis la commande originelle et un snapshot pris pendant le vol
retrouvent le même identifiant, la même échéance d'impact, puis le même hash
après la résolution.

## Prochaine preuve requise

Pour chacune des 16 armes à projectile sans vitesse publique, ainsi que pour la
Guérisseuse, il faut soit une spécification Supercell versionnée, soit une
capture client/replay reproductible qui mesure lancement, position/vol, impact,
cible détruite et ordre au tick. Pour la Tour de l’enfer, il faut en priorité
l’ordre de résolution du rayon et le re-ciblage. Une animation GUI ne sera
jamais utilisée pour combler une donnée de combat.

## Piste publique écartée le 17 septembre 2026

Le dépôt secondaire
[`Statscell/clash-of-clans-data`](https://github.com/Statscell/clash-of-clans-data)
se présente comme un formateur de données du jeu et publie un `output/raw.json`.
La révision consultée n'y expose toutefois que `RAW_UNITS` et
`RAW_SUPER_UNITS` : ni table de projectiles, ni vitesse de projectile, ni
identifiant de build associé à ces valeurs. Il ne peut donc pas compléter ce
contrat de trajectoire et n'est pas ajouté aux provenances de statistiques du
catalogue. La source demeure secondaire et non affiliée à Supercell.

Le dépôt secondaire
[`Enjoyop2/Clash-of-Clans-data-assets`](https://github.com/Enjoyop2/Clash-of-Clans-data-assets)
a également été contrôlé. Il annonce des CSV et assets historiques, mais son
index public s'arrête à `v18.200.9` et son propre README indique que des
versions peuvent manquer. Son `logic/projectiles.csv` expose bien une colonne
interne `Speed` (notamment `Cannonball=1200`, `Mortar Ammo=500` et
`HealerEnergy=1200`). Sa table `traps.csv` associe la Mine aérienne chercheuse
à `LargeDarkElixirBalloon`, dont la vitesse est `350` : la concordance avec
les 3,5 cases/s Supercell est compatible avec une échelle historique de
100 unités internes par case. C'est une calibration du seul snapshot
`v18.200.9`, non une preuve que les autres vitesses ou les règles de vol sont
encore identiques dans le build TH18 figé. Sans snapshot de build TH18
correspondant, il ne documente pas les valeurs de vol courantes : il est
seulement enregistré dans le delta documentaire comme piste historique
insuffisante. La calibration et les exemples non appliqués sont figés
séparément dans
`data/reference/historical-projectile-speed-calibration-v18.200.9.json`.

Cette archive contient désormais une matrice comparative, elle aussi
strictement non appliquée : les munitions historiques reliées aux troupes et
défenses exécutables y sont enregistrées avec leur vitesse brute et leur
conversion conditionnelle. Elle couvre notamment Archer (14), Sorcier (5/9),
Guérisseuse (12), Serviteur (3,8), Bébé dragon (4,75), Dragon Rider (9,6),
Canon (12), Tour d'archers (18/35), Mortier (5), Tour de sorciers (5/9),
Défense aérienne (8), X-Bow (23/24/25), Tour à bombes (8) et Aigle (5), en
cases/s après calibration. Dragon et Tesla y figurent explicitement comme
champs `Projectile` vides dans l'archive ; ce n'est pas une preuve d'impact
instantané. Aucun de ces nombres ne peut alimenter `GameData` tant qu'une
source courante versionnée n'a pas fixé le niveau, le mode, le lancement, la
collision et la perte de cible.

L'archive brute que ce dépôt référence à l'adresse
`storage.googleapis.com/game-assets-clashofclans.appspot.com/raw_json.zip` a
en outre refusé l'accès direct le 17 septembre 2026 (HTTP 403). Sans archive
accessible, sans empreinte et sans version de client, elle ne peut pas être
gelée comme source reproductible.
