# Addendum projectile — Inferno Dragon

Le 18 septembre 2026, `inferno_dragon` (Inferno Dragon) entre dans le périmètre de faisceau
exécutable avec un contrat distinct de la Tour de l'Enfer. Les notes Supercell
établissent une cadence de 0,6 s et un ralentissement ultérieur de rampe; une
table secondaire séparée donne les paliers 1,7/3,2 s et les DPS par niveau.
Le Core les quantifie à `kTickMs`, conserve le verrou de cible dans l'état
sérialisé et émet `inferno beam` via les mêmes événements Core consommés par
le Viewer et RL.

La remise à zéro lors d'un changement, décès ou éloignement de cible et
l'impact direct sont des approximations déterministes documentées. La source
publique disponible ne fixe ni portée mesurée, collision, altitude,
interaction de sorts, ordre client au tick ni rendu du faisceau; cette voie
reste donc partielle et n'est pas une validation de fidélité.
