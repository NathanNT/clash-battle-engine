# Addendum Monolithe — audit de projectile TH18

Date : 2026-09-18. Cet addendum est une référence additive : il préserve
l'audit et le contrat figés du 17 septembre, dont les SHA-256 sont épinglés
dans `projectile-contracts-2026-09-18-monolith.json`.

Le Monolithe devient le 23e chemin d'arme à projectile exécutable parmi les 59
candidats longue portée hors Guérisseuse; 36 candidats restent non exécutables.
La table secondaire Fandom consultée le 18 septembre donne un impact additionnel
de 11, 12, 13, 14 et 15 % des PV maximum de la cible pour les niveaux 1 à 5.
Le Core ajoute ce montant à l'impact de base au tick du tir et stocke le résultat
dans `BattleState::Projectile`; ni le GUI ni RL ne le recalculent. Le test dédié
vérifie 225 + 11 % de 1 900 = 434, y compris après snapshot et replay.

La source est secondaire; une table officielle numérique versionnée reste
requise. Les délais de lancement, vol, perte de cible et impact ne sont pas
publiés pour le build gelé : le modèle logique `T+10 ms` demeure explicitement
non fidèle. Aucun profil historique v18.200.9 reproductible n'est appliqué.
