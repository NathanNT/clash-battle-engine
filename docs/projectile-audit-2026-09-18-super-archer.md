# Addendum projectile — super archer

Le 18 septembre 2026, Super Archer est ajouté au périmètre de projectile
exécutable. La source secondaire consultée distingue l'acquisition à 6 cases
du Sharp Shot droit et pénétrant de 12 cases. Le Core fige son extrémité au
lancement, puis applique les impacts dans l'ordre du rayon avec égalité par
identifiant. L'extrémité et le flag de pénétration sont sérialisés en Snapshot
V16 et couverts par replay.

La vitesse, la largeur exacte du rayon, les hitboxes, l'altitude, la perte de
cible et l'ordre client ne sont pas publiés. L'impact logique T+10 ms et les
rayons d'entité Core sont donc explicitement partiels; ils ne valident pas la
fidélité graphique ou client.
