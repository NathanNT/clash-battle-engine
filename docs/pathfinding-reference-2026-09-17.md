# Référence de pathfinding Home Village — 2026-09-17

## Portée et état de la preuve

La référence de combat est le Home Village, consultée le 17 septembre 2026.
Supercell ne publie pas l'algorithme de navigation interne de Clash of Clans.
Les notes officielles attestent de corrections de ciblage et de déblocage de
troupes, mais ne décrivent pas le graphe, les coûts ni les égalités. Une étude
secondaire récente le confirme explicitement et propose une approximation par
réévaluation locale ; une documentation communautaire décrit le choix observé
entre contourner et casser un mur.

| Provenance | Consultation | Usage dans CoCSim |
| --- | --- | --- |
| [Notes de mise à jour Supercell — Builder Base 2.0](https://supercell.com/en/games/clashofclans/blog/release-notes/full-patch-notes-bb2/) | 2026-09-17 | Corroboration officielle limitée : le ciblage/anti-blocage est un comportement maintenu ; aucune règle Home Village détaillée n'en est déduite. |
| [Preprints, *Pathfinding and Target Selection in a Clash-of-Clans-Style Combat Environment*](https://www.preprints.org/manuscript/202601.2386) | 2026-09-17 | Source secondaire : absence d'algorithme propriétaire public et modèle local observé. |
| [Goblins Farm — Walls and pathing](https://goblinsfarm.com/wiki/mechanics/walls-and-pathing.html) | 2026-09-17 | Source secondaire : mur à casser ou détour selon accessibilité/coût ; pas une spécification officielle. |

La fidélité de l'algorithme propriétaire exact est donc **non vérifiable**.
CoCSim ne prétend pas l'avoir reconstitué. Les résultats ci-dessous sont une
implémentation déterministe, continue et testée du comportement observable.

## Règle actuellement implémentée

`BattleState::next_path_waypoint` est l'unique autorité (Core, donc partagée
par headless, GUI et RL) :

1. les bâtiments vivants bloquent leurs cases selon leur emprise importée ; les
   pièges ne les bloquent pas et les murs couverts par Saut sont ouverts ;
2. un segment direct est accepté s'il ne traverse aucune emprise, dilatée du
   rayon de la troupe ; la position reste continue, donc une troupe peut marcher
   en diagonale ;
3. sinon, Dijkstra déterministe sur les cases établit seulement la topologie
   du détour ; le tas min ordonné par `(coût, index de case)` fixe les égalités ;
4. le Core choisit le point le plus éloigné de cette route directement visible,
   ce qui évite les successions de coins en L sans couper un obstacle ;
5. mouvement et réévaluation sont exclusivement à `kTickMs`, le waypoint est
   un état logique sérialisé. La GUI ne l'interpole ni ne le modifie.

Le choix d'une cible, la priorité de destruction de murs et le coût exact des
compartiments restent des approximations documentées dans
`docs/home-village-coverage.md`. Ils ne sont pas déclarés conformes au jeu tant
qu'une preuve plus précise n'est pas disponible.

## Régressions exécutables

`core_tests` couvre :

- `PATH_ANY_ANGLE` : trajectoire diagonale libre et égalité de hash après
  snapshot/restauration ;
- `PATH_ANY_ANGLE_OBSTACLE` : un mur force un contournement dont le premier
  mouvement est oblique ;
- tests existants de murs/Saut, de snapshots, replays et identité headless/GUI.

Exécuter :

```powershell
ctest --test-dir build -C Debug --output-on-failure
```
