# Addendum projectile — Ricochet Cannon et Tour de Super Sorcier

`ricochet_cannon` émet la cible primaire et un unique rebond sol déterministe,
à 70 % dans le rayon officiel de 3,5 cases. `super_wizard_tower` émet une
frappe primaire puis au plus quinze branches, à 40 %, dans le rayon secondaire
de quatre cases. Dans les deux cas, Core capture les victimes au lancement,
mesure depuis la cible primaire et départage distance puis identifiant.

Chaque frappe reste un projectile logique sérialisé à impact T+10 ms. Les
sources ne donnent pas l'éligibilité exacte, le bord d'emprise, la trajectoire,
la vitesse, la collision, la perte de cible ou l'ordre client : ce choix est
donc un socle reproductible partiel, jamais une mesure du client.
