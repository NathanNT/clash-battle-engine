# Matrice de couverture Home Village

Depuis la [migration du pas fixe](tick-migration-2026-09-23.md), le Core utilise
16 ms. Les mentions `T+10 ms` et les exemples de quantification à 10 ms dans
les lignes historiques ci-dessous décrivent le comportement avant migration ;
leur équivalent exécutable actuel est le tick suivant de 16 ms. Les données de
référence et les lacunes de fidélité ne changent pas.

Légende : `oui` = câblé dans le dépôt ; `partiel` = présent mais non fidèle au
comportement complet ; `non` = absent. `a_verifier` ne doit jamais être interprété
comme terminé. La référence et ses limites sont dans
[`reference/home-village-reference-2026-09-17.md`](reference/home-village-reference-2026-09-17.md).
Le registre machine-lisible des lacunes de documentation est
[`th18-documentation-delta-2026-09-17.json`](../data/reference/th18-documentation-delta-2026-09-17.json) ;
il est validé par `validate_th18_documentation_delta.py`. Une lacune de ce
registre bloque la colonne « Fidélité », même lorsque l'élément est chargeable.
Les corrections après gel sont conservées, anciennes valeurs comprises, dans le
[`ledger de révisions`](../data/reference/th18-2026-09-17-revisions.json) et
vérifiées avec la référence : elles ne remplacent donc jamais une donnée
historique silencieusement. L'audit détaillé du chemin de projectiles
exécutable (modèle Core, GUI/RL et lacunes par arme) est dans
[`projectile-audit-2026-09-17.md`](projectile-audit-2026-09-17.md).
Le registre associé [`combat-damage-paths-2026-09-17.json`](../data/reference/combat-damage-paths-2026-09-17.json)
recense aussi sorts, pièges, contacts et explosions : aucun de ces chemins ne
doit être confondu avec une trajectoire de projectile.

Les vitesses de mouvement conservent la valeur affichée dans le catalogue, mais
le moteur emploie une valeur physique dérivée dans
[`movement-speed-calibration-2026-09-17.json`](../data/reference/movement-speed-calibration-2026-09-17.json) :
`vitesse / 8` cases par seconde. Cette conversion est une source secondaire
explicitement incertaine ; elle est néanmoins appliquée par le même `GameData`
immuable dans Core, Viewer et RL, et une régression couvre une seconde de
déplacement ainsi que snapshot/restauration.

Le périmètre du but persistant est fixé séparément dans
[`th18-permanent-scope-2026-09-18.json`](../data/reference/th18-permanent-scope-2026-09-18.json) :
il contient 107 éléments matérialisables, plus l'Electromite requis de Super
Yeti, explicitement inventorié mais absent du catalogue et donc non
matérialisable. Il exclut explicitement les Crafted Defenses temporaires.
`audit_th18_conformity.py` émet une matrice individuelle
pour ce périmètre : inventaire, données/niveaux, Core, comportement, headless,
GUI, RL, assets, snapshot/replay, tests, fidélité et incertitudes. Les voies
partagées et les preuves absentes sont distinguées; aucune ligne ne devient
fidèle par simple présence dans le catalogue.

| Famille | Inventaire de référence | Données / provenance | Moteur / Viewer | RL | Tests | Fidélité |
|---|---|---:|---:|---:|---:|---:|
| Référence TH18 gelée | [registre 2026-09-17](../data/reference/th18-2026-09-17.json) : deltas officiels datés + snapshot secondaire identifié; [vérification additive 2026-09-18](../data/reference/th18-reference-verification-2026-09-18.json) avec empreinte SHA-256 du registre figé | inventaire 197/197, primaire_partiel + secondaire_tracé | n/a | n/a | `validate_th18_reference.py`, `validate_th18_reference_verification.py`, `validate_raw_catalogue_consistency.py`, `validate_th18_documentation_delta.py` | inventaire_données_complet_comportements_ouverts |
| Troupes déjà présentes | barbarian, archer, giant, wall_breaker, balloon, wizard, healer, dragon, baby_dragon, miner, pekka, golem, goblin, minion, hog_rider, valkyrie, dragon_rider, golemite (invoqué) | niveaux TH18 importés ; [Wall Breaker L1–14, x40 Mur + rayon 2](../data/reference/th18-2026-09-19-wall-breaker-baseline.json) ; [Healer L1–11, portée 4,5 et soins sol uniquement](../data/reference/th18-2026-09-19-healer-baseline.json) ; [Électro Titan L1–5, aura 3,5/0,4](../data/reference/th18-2026-09-19-electro-titan-baseline.json) ; [Lanceur L1–4](../data/reference/th18-2026-09-19-thrower-baseline.json) ; [Barbarian L1–13](../data/reference/th18-2026-09-19-barbarian-baseline.json) ; [Archer L1–14, portée 3,5](../data/reference/th18-2026-09-19-archer-baseline.json) ; [Giant L1–14, priorité Défenses](../data/reference/th18-2026-09-19-giant-baseline.json) ; [Dragon L1–13, portée 2,5](../data/reference/th18-2026-09-19-dragon-baseline.json) ; [Balloon L1–13, splash 1,2](../data/reference/th18-2026-09-19-balloon-baseline.json) ; [Golem L1–15 + Golemite](../data/reference/th18-2026-09-19-golem-golemite-baseline.json) ; [Wizard L1–14, splash 0,3](../data/reference/th18-2026-09-19-wizard-baseline.json) ; [Bébé Dragon L1–12 + Tantrum](../data/reference/th18-2026-09-19-baby-dragon-tantrum-contract.json) ; [P.E.K.K.A L1–13](../data/reference/th18-2026-09-19-pekka-baseline.json) ; [Gobelin L1–10, priorité Ressources](../data/reference/th18-2026-09-19-goblin-baseline.json) ; [Minion L1–14 + immunité Mine chercheuse](../data/reference/th18-2026-09-19-minion-baseline.json) ; [Hog Rider L1–15, défenses + Murs](../data/reference/th18-2026-09-19-hog-rider-baseline.json) ; Mineur L1–12 + tunnel déterministe ; Golemite L1–15 tracé et invoqué, position de spawn exacte non publiée | partiel | partiel | core_behavior + RL catalogue | non_conforme |
| Root Rider — L1–4 | root_rider | [Delta 2026-09-19](../data/reference/th18-2026-09-19-root-rider-baseline.json) : L1–3 HP/DPS officiels post-équilibrage, L4 TH18 officiel mais valeurs secondaires ; priorité Défenses officielle, contact Mur 4 000 / 0,4 s secondaire. | partiel — même route Core/CLI/Viewer vers la Défense puis contact Mur | partiel — même Core via palette/scénario | test ciblé + snapshot/replay/hash + CLI + RL runtime | non_conforme : choix de Mur, route, collision et retargeting non vérifiés |
| Lanceur | thrower L1–4 | [Delta 2026-09-19](../data/reference/th18-2026-09-19-thrower-baseline.json) : table build 18.600.3 (PV, DPS, cadence 2,5 s, portée 6, logement 16, vitesse 18, cibles air/sol) ; L4 (2 800 PV / 240 DPS) confirmé par Supercell. | partiel — même projectile mono-cible Core/CLI/Viewer ; priorités héros/renforts hors périmètre | partiel — même Core via palette/scénario | test ciblé + snapshot/replay/hash + CLI + RL runtime | non_conforme : trajectoire/timing, collision, géométrie et priorités spéciales non vérifiés |
| Électro-titan | electro_titan L1–5 | [Delta 2026-09-19](../data/reference/th18-2026-09-19-electro-titan-baseline.json) : PV/frappe L1–5, aura rayon 3,5 et cadence 0,4 ; sources secondaires : aura sol/air sans Murs, insensible à Gel et Rage. | partiel — aura périodique partagée Core/CLI/Viewer ; première impulsion, cibles cachées/souterraines et ordre exact ouverts | partiel — palette/scénario appellent le même Core | test ciblé + snapshot/replay/hash + CLI + RL runtime | non_conforme : chronologie fine et projectile de fouet non vérifiés |
| Electro Dragon | electro_dragon L1–9 | The frozen numeric L8/L9 correction and 2.5-tile range remain preserved. The additive 2026-09-18 chain reference records the primary five-victim cap and no-repeat guarantee, plus secondary 20% decay and one-empty-tile geometry. | partial: Core creates sequential serialized projectiles, and headless, Viewer, and RL consume that same state | partial: same Core | focused behavior, snapshot/replay, CLI, RL catalogue, projectile audit and asset validation | not conformant: exact jump metric, flight and target-loss timing, collision, and death-lightning seed, radius and schedule remain open |
| Bowler | bowler L1–10 | The additive 2026-09-18 reference preserves the raw L10 value and corrects it to the dated build-18.600.3 table. Primary evidence establishes two impacts through Super Bowler's documented extra third bounce. | partial: Core serializes two fixed ground impact points on the initial ray. Headless, Viewer, and RL consume that shared state. | partial: same Core | focused behavior, snapshot/replay, CLI, RL catalogue, projectile audit, and asset validation | not conformant: the three-tile forward step is a documented proxy and bounce path, splash radius, collision, flight, walls, target loss, and timing remain open |
| Ice Golem | ice_golem L1–9 | The additive 2026-09-18 reference imports the official death-triggered Freeze contract, official L1–5 and L7 durations, and explicitly secondary L6/L8/L9 and 7.5-tile radius values. | partial: attacker death creates the shared serializable Core Freeze effect. Headless, Viewer, and RL consume that effect without duplicated rules. | partial: same Core | focused behavior, snapshot/replay, CLI, RL catalogue, and asset validation | not conformant: propagation, expansion delay, exact target eligibility, reset semantics, and tick order remain open |
| Apprentice Warden | apprentice_warden L1–4 | The additive 2026-09-18 reference imports the primary 7-tile Life Aura and 24, 26, 28, and 30 percent HP increases. | partial: Core computes the strongest nearby attacker aura for ground troops. Headless, Viewer, and RL consume the resulting shared Core state. | partial: same Core | focused behavior, snapshot/replay, CLI, RL catalogue, and asset validation | not conformant: entry and exit timing, removal damage semantics, exact eligible spawned units, self-targeting, and visual radius remain open |
| Data-only troops | meteor_golem | Their levels are catalogued in the frozen snapshot, but their specific behaviour contracts are not integrated yet. | no | no | audit_structure | behavior_missing |
| Lava Hound / Lava Pup | lava_hound L1-8, lava_pup L1 required | The [20 September additive baseline](../data/reference/th18-2026-09-20-lava-hound-lava-pup-baseline.json) retains the parent direct contract and adds a spawned-only 50-HP/35-damage Lava Pup plus the source-observed 8–15 offensive split counts. Core queues children in stable order on attacker death; the circular formation and timing are not claimed. | partial: same Core through Headless, Viewer, and RL; child visual source remains unverified | partial: same Core | `lava_hound_tests`, snapshot/replay, Headless CLI, RL runtime, and asset validation | not conformant: parent explosion, split geometry/timing, projectile collision, target ordering and visual fidelity remain open |
| Assets des sous-unités invoquées — recontrôle 20 septembre | lava_pup, ice_pup, yetimite, druid_bear, super_hog, super_rider, electromite | La [delta d’assets](../data/reference/th18-2026-09-20-spawned-subunit-assets-recheck.json) conserve des paliers visuels publics (Bear L1–2/L3–4/L5/L6, Yetimite jusqu’à L8, Lava Pup et Super Rider non nivelés) et distingue les concepts/bêtas Ice Pup. Les fichiers directs sont indisponibles, sans hash ni build ; aucun asset applicatif n’est copié ou déduit. | partiel — routes Core existantes inchangées | partiel — le Viewer conserve ses fallbacks | chaîne active hash-pinnée | non_conforme : aucun asset source stable, versionné et vérifiable |
| Headhunter | headhunter L1-4 | The 18 September additive delta retains parent data and adds official TH18 level 4. An unversioned secondary table provides direct damage, Hero priority, the x4 multiplier, and poison values. | partial: the parent attacks buildings through the shared fallback when no out-of-scope Hero is present. Hero priority remains immutable Core metadata. | partial: same Core through Headless, Viewer, and RL | `headhunter_tests`, snapshot/replay, Headless CLI, RL runtime, and asset validation | not conformant: Heroes, x4 damage, poison, wall hopping, projectile, and visual fidelity remain open |
| Druid / Bear | druid L1-6, required druid_bear L1-6 | The 18 September additive delta preserves primary two-form lifecycle evidence, adds the officially confirmed TH18 Druid L6, and records current secondary human and Bear tables. | partial: shared Core uses human healing metadata, the 30-second transform deadline, human-defeat Bear spawning, and the Spring Trap ejection exception. | partial: same Core through Headless, Viewer, and RL | `druid_tests`, snapshot/replay, Headless CLI, RL runtime, and asset validation | not conformant: healing chains, transform health and tick order, projectile and spawn geometry, plus Bear assets remain open |
| Furnace / Firemite | furnace L1-4, required firemite L1-4 | The 18 September additive delta preserves primary stationary-Furnace and Firemite role evidence, current parent lifetime and maximum counts, plus current child values. | no: no exact deterministic child schedule, parent decay, or persistent-flame contract is available, so Core does not substitute one. | no: no shared Core mechanic exists to expose. | inventory and provenance validation | not conformant: spawn schedule, self-decay, fire geometry, child targeting and assets remain open |
| Furnace / Firemite — suivi 20 septembre | furnace L1-4, firemite L1-4 | La [delta additive de suivi](../data/reference/th18-2026-09-20-furnace-firemite-followup.json) conserve l’export technique historique (sans build vérifiable) : ses délais ne suffisent pas à définir le calendrier, le saut ou le feu. Aucun comportement n’est activé. | non | non | chaîne active hash-pinnée | partiel — cycle et feu persistants restent non matérialisables |
| Furnace / Firemite — cycle revérifié le 20 septembre | furnace L1-4, firemite L1-4 | La [delta additive de cycle](../data/reference/th18-2026-09-20-furnace-firemite-lifecycle-recheck.json) ajoute l’annonce publique : fenêtre 60 s, plafonds 19/20/21/22, auto-dégâts, enfants perdus si le parent est détruit et feux non cumulables. Supercell confirme aussi que les feux atteignent les engins de siège. « Environ une toutes les 3 s » n’est pas un calendrier déterministe ; délai initial, cadence exacte, décroissance, saut/collision et géométrie/pulsations du feu restent absents. | non | non | chaîne active hash-pinnée | partiel — aucune interpolation de calendrier ou de feu n’est activée |
| Super Troupes — Sneaky Goblin | Core L7–9 ; L10 catalogue uniquement | La [delta 2026-09-19](../data/reference/th18-2026-09-19-sneaky-goblin-baseline.json) épingle les L7–9 historiques, le L10 secondaire et les contrats Supercell : Cloak 5 s après déploiement et dégâts ressources ×2. Core, Headless, Viewer et RL partagent L7–9 ; L10 reste explicitement indisponible dans ces adaptateurs car le test historique gelé fixe le maximum à L9. | partiel — route d’asset publique, tier L10 non vérifié | partiel — même état Core L7–9 | `sneaky_goblin_tests`, snapshot/replay/hash, CLI, RL runtime | non_conforme : L10 non matérialisé, valeurs secondaires, frontière de détection, collision, route, ordre et fidélité visuelle non vérifiés |
| Super Wall Breaker | super_wall_breaker L7–14 | Le payload figé fournit tous les niveaux, impacts et dégâts à la mort. La [référence additive](../data/reference/th18-2026-09-18-super-wall-breaker.json) dérive explicitement du parent les 40× dégâts Mur et le rayon 2 cases, et la note Supercell du 30 mars 2020 établit l'explosion garantie à la cible ou à la destruction. Core/CLI/Viewer/RL utilisent le même cycle; snapshot et replay sont vérifiés. | partiel — sprite source réutilisé ; géométrie de rayon Core 2 cases | partiel — même Core; aucune animation ne décide l'explosion | `super_wall_breaker_tests`, snapshot/replay, CLI headless, runtime RL | non_conforme : dérivations secondaires, choix du Mur protégé, collision, ordre intra-tick et fidélité visuelle non vérifiés |
| Super Barbarian | super_barbarian L8–12 (L8 applicable TH18) | La [référence additive](../data/reference/th18-2026-09-18-super-barbarian.json) conserve la chaîne historique : Supercell (30 mars 2020) confirme l'augmentation vitesse/dégâts pendant les huit premières secondes; +70 % dégâts et +16 vitesse proviennent d'une table communautaire sans build identifié. Core conserve l'échéance T+ms en Snapshot V15; GUI et RL ne lisent que cet état Core. | partiel — image source `barbarian/super/level-8`, géométrie 1×1 Core; animation et placement visuel restent non vérifiés | partiel — même Core, état Rage de lecture seule | `super_barbarian_tests`, snapshot/replay, CLI headless, runtime RL | non_conforme : multiplicateurs secondaires, limite exacte/ordre intra-tick, cumul Rage/Haste et fidélité visuelle ouverts |
| Super Giant | super_giant L9–14 (L14 applicable TH18) | La [référence additive](../data/reference/th18-2026-09-18-super-giant.json) conserve l'annonce Supercell du 30 mars 2020 (priorité défenses, sol, frappe unique et dégâts Mur accrus); une table secondaire datée fournit ×5 par impact, dont L14 410→2 050. Core réemploie les routes de cible, chemin et dégâts; GUI/RL lisent ce même état. | partiel — image source `giant/super/level-9`, géométrie 1×1; collision/placement visuel non mesurés | partiel — même Core | `super_giant_tests`, snapshot/replay, CLI headless, runtime RL | non_conforme : multiplicateur secondaire, mur protégé, collision, ordre intra-tick et fidélité visuelle ouverts |
| Super Archer | super_archer L8–14 (L14 applicable TH18) | La [référence additive](../data/reference/th18-2026-09-18-super-archer.json) conserve les avis Supercell TH18/équilibrage; la table secondaire distingue acquisition 6 et projectile Sharp Shot fixe 12, pénétrant. Core fige le rayon au lancement et le sérialise en Snapshot V17; CLI, GUI et RL appellent ce même Core. | partiel — image source `archer/super/level-8`, géométrie 1×1; largeur de collision/placement visuel non mesurés | partiel — même Core | `super_archer_tests`, snapshot/replay, CLI headless, runtime RL | non_conforme : source secondaire, rayon de collision, vol, perte de cible, altitude, ordre précis et fidélité visuelle ouverts |
| Rocket Balloon | rocket_balloon L8–12 (L12 applicable TH18) | La [référence additive](../data/reference/th18-2026-09-18-rocket-balloon.json) rattache la fenêtre officielle de boosters aux quatre premières secondes; +52 vitesse est une dérivation secondaire explicitement tracée depuis l'équivalence Haste L5. Core sérialise l'échéance, et CLI, GUI et RL appellent le même état. | partiel — image source `balloon/super/level-8`, géométrie 1×1; collision/placement visuel non mesurés | partiel — même Core | `rocket_balloon_tests`, snapshot/replay, CLI headless, runtime RL | non_conforme : vitesse numérique secondaire, frontière/empilement, splash/dégâts mort, vol, collision et fidélité visuelle ouverts |
| Super Wizard | super_wizard L9–14 (L14 applicable TH18) | La [référence additive](../data/reference/th18-2026-09-18-super-wizard.json) fixe les impacts secondaires à 40 % depuis Supercell. Core branche depuis la cible primaire, garde dix victimes totales et un rayon de base 3 comme approximations versionnées, départagées par distance puis ID; GUI/RL utilisent les projectiles Core sérialisés. | partiel — asset source `wizard/super/level-9`, géométrie 1×1; overlay/placement non mesurés | partiel — même Core | `super_wizard_tests`, snapshot/replay, CLI headless, runtime RL | non_conforme : plafond, rayon, éligibilité, vol, ordre client et fidélité visuelle sont explicitement ouverts |
| Super Dragon | super_dragon L3–13 (ordinal courant; TH L13 ouvert) | La [référence additive](../data/reference/th18-2026-09-18-super-dragon.json) conserve le type splash air/sol officiel et remplace le cycle brut 0,192 s par 1,8 s depuis une table build 18.400.21. Core/CLI/GUI/RL font un impact agrégé sur la cible primaire; aucun rayon ou burst n'est inventé. | partiel — asset source L3 réutilisé; géométrie/placement L13 ouverts | partiel — même Core | `super_dragon_tests`, snapshot/replay, CLI headless, runtime RL | non_conforme : Roast, rayon, victimes splash, vol, collision, niveau/TH et fidélité visuelle ouverts |
| Super Minion | super_minion L8–14 (L14 applicable TH18) | La [référence additive](../data/reference/th18-2026-09-18-super-minion.json) fixe par Supercell huit Long Shots initiaux à 10,25 cases. Core/CLI/GUI/RL partagent ce compteur sérialisé et les projectiles T+10 ms; faute de multiplicateur public, les dégâts ordinaires sont un plancher explicite, jamais une valeur fidèle. | partiel — asset source `minion/super/level-8`, géométrie 1×1; collision/placement visuel non mesurés | partiel — même Core | `super_minion_tests`, snapshot/replay V19, CLI headless, runtime RL | non_conforme : multiplicateur, cycle compteur, vol, perte de cible, collision, ordre et fidélité visuelle ouverts |
| Super Valkyrie | super_valkyrie L7-12 | The [additive reference](../data/reference/th18-2026-09-18-super-valkyrie.json) imports the L12 build observation and source-backed cadence and movement. Core runs direct primary-target ground combat only. | partial: public level-7 visual tier passes asset validation, while source fidelity remains unverified. | partial: same Core | `super_valkyrie_tests`, snapshot/replay, Headless CLI, Viewer asset and RL runtime | not conformant: death Rage duration and lifecycle, first-hit delay, splash radius, priority interactions and visual fidelity remain open |
| Super Hog Rider | super_hog_rider L10-15, spawned super_hog and super_rider L10-15 | The [additive reference](../data/reference/th18-2026-09-18-super-hog-rider-split.json) imports Supercell's parent and child L10-12 tables, corrects parent L12 HP from 1750 to 1700, and records L13-15 child values as secondary version-unknown data. | partial: parent death queues one child of each kind through the shared deterministic Core spawn path. Headless, Viewer, and RL consume that state. | partial: same Core. Child assets and launch animation are not claimed. | `super_hog_rider_tests`, snapshot/replay, CLI headless, runtime RL | not conformant: exact launch geometry, timing, target inheritance, Super Hog wall jump pathing, distraction rules, and source-verified child assets remain open |
| Super Yeti / Electromite | super_yeti L3-8, electromite required | The [additive reference](../data/reference/th18-2026-09-18-super-yeti-baseline.json) adds the dated L8 parent observation and a shared direct parent baseline. The [20 September follow-up](../data/reference/th18-2026-09-20-electromite-followup.json) corroborates unversioned child L3–8 values but cannot reconcile the 400-versus-400/800 threshold evidence. | partial: public level-3 visual tier passes asset validation. Electromite has no sourced asset. | partial: same Core | `super_yeti_tests`, snapshot/replay, Headless CLI, Viewer asset and RL runtime | not conformant: Electromite level mapping, threshold, spawn timing and position, chain geometry, target selection, self-destruction, Wall jump, trap interaction, and visual fidelity remain open |
| Super Witch / Big Boy | super_witch L5-8, big_boy required | The [additive reference](../data/reference/th18-2026-09-18-super-witch-baseline.json) makes the dated parent direct baseline available. The [20 September follow-up](../data/reference/th18-2026-09-20-big-boy-followup.json) corroborates unversioned 4,100 HP/700-hit observations and the official 20-housing change, but not a deterministic child lifecycle. | partial: public level-5 visual tier passes asset validation. Big Boy has no sourced asset. | partial: same Core | `super_witch_tests`, snapshot/replay, Headless CLI, Viewer asset and RL runtime | not conformant: Big Boy level mapping, summon timing and position, coexistence, respawn, target selection, trap interaction, area splash, projectile timing, and visual fidelity remain open |
| Super Bowler | super_bowler L4–10 (L10 applicable TH18) | La [référence additive](../data/reference/th18-2026-09-18-super-bowler.json) retient les trois impacts destructeurs publiés par Supercell; L10, cadence et portée viennent d'une table secondaire datée. Core/CLI/GUI/RL partagent trois impacts splash sol sérialisés; leur pas 3 et rayon 0,6 sont des proxys secondaires explicitement versionnés. | partiel — asset source `bowler/super/level-4`, géométrie 1×1; collision/placement visuel non mesurés | partiel — même Core | `super_bowler_tests`, snapshot/replay V19, CLI headless, runtime RL | non_conforme : trajectoire, retargeting, cibles intermédiaires, délai/vol, collision, ordre et fidélité visuelle ouverts |
| Super Miner | super_miner L7-12 | The [additive reference](../data/reference/th18-2026-09-18-super-miner.json) combines Supercell's qualitative launch contract, a dated build 18.400.21 base table, and version-unknown stage and blast values. Core reuses the serialized Miner underground route, three-stage drill and ground-only death blast. | partial: public image path exists, but source and level-specific fidelity are unverified. | partial: same Core | `super_miner_tests`, snapshot/replay, CLI headless, runtime RL | not conformant: stage boundaries, lock reset, underground timing, traps, pathing, bomb delay and visual fidelity remain open |
| Inferno Dragon | inferno_dragon L6–12 (L12 applicable TH18) | La [référence additive](../data/reference/th18-2026-09-18-inferno-dragon.json) conserve les deux notes Supercell (cadence 0,6 s; ralentissement de rampe) et rattache séparément les paliers secondaires 1,7/3,2 s et DPS L12 90/182/1 820. Core ne réemploie pas le contrat Tour : il conserve le verrou sérialisé, exécute un faisceau direct déterministe et quantifie toute décision à 10 ms; CLI, GUI et RL passent par ce même Core. | partiel — image source `baby-dragon/super/level-6`, géométrie 1×1; collision/placement visuel non mesurés | partiel — même Core | `inferno_dragon_tests`, snapshot/replay, CLI headless, runtime RL | non_conforme : paliers/DPS secondaires; reset, impact direct, portée, altitude, sorts, collision et fidélité visuelle restent des approximations ou incertitudes documentées |
| Ice Hound / Ice Pup | ice_hound L5-8, spawned ice_pup L1 | The [additive reference](../data/reference/th18-2026-09-18-ice-hound-ice-pup.json) preserves the documented Air Defense priority, build-dated parent table, sourced L5-8 Pup count and a pinned secondary child table. Core, CLI, Viewer, and RL use the same parent deployment and deterministic child queue. | partial: parent source visual tier exists, Ice Pup asset and spawn placement remain unverified. | partial: shared Core | `ice_hound_tests`, snapshot/replay, CLI headless, runtime RL | not conformant: slow magnitude and duration, freeze radius, spawn geometry, projectile timing, collision and asset fidelity remain open. |
| Yeti / Yetimite | yeti L1-8, spawned yetimite L1-8 | The [additive reference](../data/reference/th18-2026-09-18-yeti-yetimite.json) records the official parent and child behavior plus dated secondary level tables. Core releases a child for each sourced 600 damage up to capacity, releases the remaining children on death, and keeps all spawn state in snapshots and replays. Yetimites prioritize defenses, use their sourced damage multipliers and splash, bypass walls only, and self-destruct after the first impact. | partial: parent is adapter-visible. Yetimite asset levels and source fidelity remain unverified. | partial: shared Core | `yeti_tests`, snapshot/replay, CLI headless, runtime RL | not conformant: child cadence, entry delay, spawn formation, wall-jump geometry, armed Town Hall multiplier and visual fidelity remain open. |
| Witch / Witch Skeleton | witch L1-8, required witch_skeleton | The [additive reference](../data/reference/th18-2026-09-18-witch-skeleton-baseline.json) makes the source-backed L1-8 parent baseline available through Core, Headless CLI, Viewer and RL. The required Skeleton is recorded in the permanent inventory but intentionally has no GameData record or generic proxy. | partial: parent visual tier is adapter-visible. Skeleton has no source-verified asset. | partial: shared Core | `witch_tests`, snapshot/replay, CLI headless, runtime RL | not conformant: Skeleton values, level mapping, count, cap, lifetime, summon timing and geometry, targeting, splash, projectile timing and visual fidelity remain open. |
| Meteor Golem / Meteormite, data only | meteor_golem L1-3, required meteormite | The [additive reference](../data/reference/th18-2026-09-18-meteor-golem-meteormite-baseline.json) preserves the frozen table and records current build-18.600.3 parent HP and damage. Official sources establish the permanent throw, split, Wall-crossing child and remerge identity. The visible child table predates the confirmed July 2026 buff and is intentionally not imported as current data. | no | no | active-reference, raw-catalogue and scope validation | behavior missing: current child values, split trigger, throw, landing, merge, invulnerability, geometry and assets remain open. |
| Meteor Golem / Meteormite — suivi 20 septembre | meteor_golem L1-3, meteormite requis | La [delta additive de suivi](../data/reference/th18-2026-09-20-meteor-golem-meteormite-followup.json) prouve que la table enfant disponible est pré-buff et ne peut pas être importée comme actuelle. Les règles de lancer, division, fusion et invulnérabilité restent insuffisamment définies. | non | non | chaîne active, catalogue brut et audit TH18 | partiel — comportement explicitement non matérialisé |
| Meteor Golem / Meteormite — 2026-09-23 evidence recheck | meteor_golem L1-3, required meteormite | The [dated recheck](../data/reference/th18-2026-09-23-meteor-golem-meteormite-evidence-recheck.json) confirms the permanent throw and remerge identity, the official July buff and the current build-pinned parent table. It finds no current child values or complete split, landing, merge, collision and tick-order contract. | no | no | active-reference, raw-catalogue and scope validation | partial, data-only; no generic attack proxy |
| Ruin Witch / Ruin Knight — données seulement | ruin_witch, ruin_knight (invoqué) | Le [contrat additif du 19 septembre](../data/reference/th18-2026-09-19-ruin-witch-knight-contract.json) réconcilie le cycle officiel (rubble, travail 4 s, invocation 2 s, animation 2 s, apparition même après défaite pendant l’invocation) avec le correctif d’août (cap 10, aucune décroissance de PV). La [preuve courante additionnelle](../data/reference/th18-2026-09-19-ruin-witch-knight-current-evidence.json) recoupe les valeurs parent L1–4 sans résoudre la géométrie. Vitesse, rubble, coordonnées d’apparition, égalités au tick, ciblage initial et collision restent absents; aucun acteur ou proxy n’est activé. | non | non | `validate_th18_reference.py`, `validate_th18_active_reference.py`, `validate_raw_catalogue_consistency.py`, `validate_th18_documentation_delta.py` | comportement_manquant_explicitement_bloqué |
| Ruin Witch / Ruin Knight — suivi 20 septembre | ruin_witch, ruin_knight (invoqué) | La [delta de suivi additive](../data/reference/th18-2026-09-20-ruin-witch-knight-followup.json) réexamine les notes officielles de juin/août et une table build 18.600.3 : aucune ne définit la géométrie du rubble, la coordonnée « devant elle », collision, départage ou ordre au tick. Le [suivi de mouvement](../data/reference/th18-2026-09-20-ruin-witch-movement-followup.json) importe seulement vitesse 12 depuis une source secondaire qui reste obsolète pour le cap et la décroissance; les valeurs officielles 10/0 prévalent. Aucun acteur ni proxy n’est activé. | non | non | chaîne active hash-pinnée et validations de référence | partiel — comportement explicitement non matérialisé |
| Sorts déjà présents | rage, heal, lightning, freeze, haste, jump, earthquake | niveaux et provenance importés ; les effets actifs exposent rayon et expiration T+ms depuis Core | partiel | partiel — niveau de réserve réel et catalogue immuable (rayon, dégâts/soins, modificateurs, durée, pulsations) | Core + RL runtime exhaustif + snapshot/replay | a_verifier |
| Sorts données seulement | clone, invisibility, recall, poison, skeleton, bat, overgrowth, ice_block, revive, totem | niveaux normaux importés du snapshot historique figé ; interactions et unités invoquées non activées | non | non | audit_structure | comportement_manquant |
| Sort Angry — données seulement | angry | Artefact secondaire épinglé : rayon 6, logement 1, usine de sorts noirs 8, L1–4 (durées 7/8/10/12 s, coûts/temps/prérequis). Le contrat qualitatif officiel (re-ciblage défense, immunité siège, fenêtre 5 s, sans dégâts) est conservé séparément. L'ordre déterministe reste ouvert. | non | non | `validate_th18_reference.py`, `validate_raw_catalogue_consistency.py`, `validate_th18_documentation_delta.py` | comportement_manquant |
| Défenses déjà présentes | cannon, archer_tower, mortar, inferno_tower, air_defense, wizard_tower, x_bow, bomb_tower, hidden_tesla | X-Bow L13 importé ; rechargement/mise à sec non simulé. Inferno mono : paliers 0/1,5/5,25 s et reset sur perte de verrou, issus d’une source secondaire ; son attaque est maintenant un rayon direct déterministe, visualisé en violet sans projectile fictif. Air Defense a désormais une preuve ciblée de filtrage air, départage ID, projectile logique, snapshot/replay, CLI et sélection de son emprise Core 3×3. Gel/munitions et ordre de re-ciblage restent ouverts. | partiel | partiel | partiel | non_conforme |
| Eagle Artillery autonome — historique, hors TH18 | eagle_artillery | La [delta de portée](../data/reference/th18-2026-09-19-eagle-artillery-th18-scope.json) préserve les données et tests historiques, mais Supercell confirme sa fusion avec l’Hôtel de Ville au TH17. Elle n’est donc pas comptée comme défense permanente autonome au TH18; l’arme de Town Hall est suivie séparément. | sans objet | sans objet | validation de référence | hors_portée_TH18 |
| Tour d’archers multiple | multi_archer_tower L1-4, Supercharge L1-2 | La [delta additive](../data/reference/th18-2026-09-19-multi-archer-tower-baseline.json) applique les trois flèches officielles : trois cibles distinctes, trois flèches sur une cible seule et deux sur une cible quand elles ne sont que deux. Portée 10, air/sol et cadence actuelle 0,6 s sont tracés. Core réplique la cible primaire distance/ID lorsque deux cibles seulement sont présentes ; Headless, Viewer et RL lisent les mêmes projectiles. | partiel — trajectoire/collision et allocation client à deux cibles non mesurées | partiel — même Core | `multi_archer_tower_tests`, snapshot/replay/hash + audit projectile | non_conforme : sélection/alternance client, vol, collision et ordre précis ouverts |
| Canon à engrenages multiple | multi_gear_tower L1-3, Supercharge L1-2 | La [delta additive](../data/reference/th18-2026-09-19-multi-gear-tower-baseline.json) rattache les deux modes officiels, cible unique air/sol, au payload épinglé : longue portée 12/cadence 1 s et rafale rapide portée 8, quatre tirs à 0,192 s puis pause 0,383 s. Core quantifie explicitement les deux intervalles sur son tick de 10 ms (190/380 ms) et le même chemin sert Headless, Viewer et RL. | partiel — projeté à T+10 ms; vol/collision/asset exacts inconnus | partiel — sélection, pertes de cible, égalités et bascule client inconnues | `multi_gear_tower_tests`, snapshot/replay/hash, CLI, audit projectile | non_conforme : valeurs/timing secondaires, quantification Core et fidélité client restent partiels |
| Catapulte | scattershot L1-7, Supercharge L1-2 | La [delta additive](../data/reference/th18-2026-09-19-scattershot-baseline.json) rattache le projectile air/sol officiel (portée 3–10) et son éclatement derrière l’impact aux valeurs figées et au cône secondaire de 90° (décroissance à 1/5 cases, même altitude). Core sérialise un projectile logique T+10 ms puis le cône déterministe; Headless, Viewer et RL lisent ce même état. | partiel — vol/collision/asset précis inconnus | partiel — sélection, perte de cible, frontières et ordre client inconnus | `scattershot_tests`, snapshot/replay/hash, CLI, audit projectile | non_conforme : munitions/rechargement, vol, collision et ordre au tick non sourcés |
| Tour à bombes | bomb_tower L1-13 | La [delta additive](../data/reference/th18-2026-09-19-bomb-tower-baseline.json) consolide la défense de zone sol et l’explosion de destruction retardée d’une seconde, rayon 2,75, sol uniquement. Core sérialise projectile et explosion future ; Headless, Viewer et RL lisent ce même état. | partiel — trajectoire/collision/asset précis inconnus | partiel — sélection, frontières et ordre client inconnus | `bomb_tower_tests`, snapshot/replay/hash, CLI | non_conforme : table courante avec build client, vol, collision, égalités et ordre au tick non sourcés |
| Tour de l’Enfer | inferno_tower L1-12, Supercharge L1-2 | La [delta additive](../data/reference/th18-2026-09-20-inferno-tower-baseline.json) conserve L12 TH18 officiel, les portées primaire 9 mono / 10 multi, les paliers L12 155/330/3 300 DPS et les charges actuelles 165/350/3 500 DPS, 5 100/5 300 PV. Core sérialise le verrou du faisceau direct et le mode; Headless, Viewer et RL lisent le même état. | partiel — état Supercharge exposé sans sprite source exact; aucun projectile fictif | partiel — acquisition, perte de cible, limites de collision et ordre client inconnus | `inferno_tower_tests`, snapshot/replay/hash, CLI, runtime RL | non_conforme : sélection exacte du faisceau, collision, ordre au tick, persistance des munitions/rechargement et fidélité visuelle non sourcés |
| X-Bow | x_bow L1-13, Supercharge L1-2 | La [delta additive](../data/reference/th18-2026-09-19-x-bow-baseline.json) corrige les Supercharges TH18 actuelles (5 000/5 100 PV, 255 DPS) et rattache les modes : sol à 14 cases ; air+sol à 11,5. Core choisit le mode avant combat et sérialise les projectiles ; Headless, Viewer et RL lisent cet état commun. | partiel — projectile logique sans vol/collision client mesuré | partiel — sélection, perte de cible, limites et ordre client inconnus | `x_bow_tests`, snapshot/replay/hash, CLI, runtime RL | non_conforme : état chargé/épuisé, rechargement, trajectoire, collision, égalités et ordre au tick non sourcés |
| Monolithe | monolith | La [référence additive L5](../data/reference/th18-2026-09-18-monolith-l5.json) conserve le gel du 17 septembre, importe L5 (5 959 PV, 225 DPS, TH18) avec observation secondaire hashée et la table secondaire 11–15 % des PV maximum de la cible. L'[addendum de contrat projectile](projectile-audit-2026-09-18-monolith.md) conserve séparément l'audit du 17 septembre. Le Core fige l’impact calculé dans son projectile au tick de tir ; le même comportement passe par scénario/headless, Viewer et RL, avec snapshot et replay. | partiel — emprise Core 3×3 ; sprite L5 non documenté | partiel — adaptateurs du même Core ; contrat de vol non mesuré | `monolith_tests`, snapshot/replay, CLI headless, audit projectile | non_conforme : pourcentage secondaire, timing/vol/perte de cible, asset L5 et fidélité visuelle ouverts |
| Air Sweeper — données seulement | air_sweeper L1-7 | La [preuve additive courante](../data/reference/th18-2026-09-19-air-sweeper-current-evidence.json) importe avec provenance complète les PV L1-7, force 1,6–4,0 cases, portée 1–15, cadence 5 s, emprise 2×2, cible air et absence de dégâts. Supercell confirme direction choisie et cône ; la source secondaire décrit une poussée normale à une courbe, mais ne publie ni cette courbe, ni son origine, collisions, ordre multi-cible ou bords de cône. Le [recontrôle du 20 septembre](../data/reference/th18-2026-09-20-air-sweeper-mechanics-recheck.json) constate en plus des descriptions incompatibles de 10/30/60/90/120 degrés. L'[audit historique](../data/reference/th18-mechanics-audit-2026-09-18.json) est préservé. Aucun proxy de poussée n’est activé. | non | non | `validate_th18_active_reference.py` + `validate_raw_catalogue_consistency.py` + `validate_th18_mechanics_audit.py` | comportement_manquant_explicitement_bloqué |
| Spell Tower — données seulement | spell_tower L1-4 | La [preuve additive courante](../data/reference/th18-2026-09-19-spell-tower-current-evidence.json) préserve par mode les rayons, durées, recharges et effets Rage/Poison/Invisibilité/Earthquake, ainsi que L4 Earthquake TH18 confirmé par Supercell. Les règles de cible, délai, impact, pulsations, recouvrement, destruction, rechargement, visibilité et ordre au tick restent insuffisamment définies. Les sorts offensifs existants ne servent donc pas de proxy défensif. | non | non | `validate_th18_active_reference.py` + `validate_raw_catalogue_consistency.py` + `validate_th18_documentation_delta.py` | comportement_manquant_explicitement_bloqué |
| Firespitter — données seulement | firespitter L1-3 | La [preuve additive courante](../data/reference/th18-2026-09-19-firespitter-current-evidence.json) préserve L3 TH18, la rafale de 20 tirs, portée 16, intervalle 0,064 s et les valeurs par niveau. Supercell confirme le cône directionnel, l’imprécision et les projectiles touchant derrière la cible. La dispersion, les trajectoires, la collision, la sélection, l’ordre à deux impacts et la dérivation du DPS publié restent inconnus : aucune arme générique n’est activée. | non | non | `validate_th18_active_reference.py` + `validate_raw_catalogue_consistency.py` | comportement_manquant_explicitement_bloqué |
| Builder’s Hut — données seulement | builders_hut L1-8, Builder requis | La [preuve additive courante](../data/reference/th18-2026-09-19-builders-hut-current-evidence.json) enregistre les valeurs L2–8, tourelle, portée 7, réparation 0,75 s, vitesse 20 et Supercharge TH18. Le [suivi multi-Builder](../data/reference/th18-2026-09-20-builders-hut-multi-builder-followup.json) ajoute les efficacités secondaires 100/100/90/90/70 % pour 1–5 Builders, sans inventer l’allocation par tick. Le Builder est non ciblable et s’arrête avec sa cabane, mais son spawn, trajectoire, priorité, collision et ordre avec les sorts restent inconnus ; aucun faux soigneur n’est activé. | non | non | `validate_th18_active_reference.py` + `validate_raw_catalogue_consistency.py` | comportement_manquant_explicitement_bloqué |
| Ricochet Cannon | ricochet_cannon L1-4, Supercharge L1-2 | La [delta additive](../data/reference/th18-2026-09-19-ricochet-cannon-baseline.json) applique le rebond officiel unique dans 3,5 cases à 70 % de dégâts. Core sélectionne la seconde cible sol par distance puis ID ; Headless, Viewer et RL emploient ce même chemin. Le test ciblé couvre rayon, dégâts, absence hors rayon, snapshot/replay/hash. | partiel — projectile logique partagé ; trajectoire et collision non mesurées | partiel — même Core | `ricochet_cannon_tests`, snapshot/replay/hash | non_conforme : choix exact en jeu, trajectoire/vitesse, collision, ordre et assets de rebond ouverts |
| Tour de Super Sorcier | super_wizard_tower L1-2, Supercharge L1-2 | La [delta additive](../data/reference/th18-2026-09-19-super-wizard-tower-baseline.json) conserve les valeurs L1-2/Supercharge du payload gelé et applique l’annonce Supercell (fusion TH18, jusqu’à quinze ennemis proches). Le contrat secondaire courant fixe portée 8, cadence 1,3 s, rayon de branche 4 et 40 % de dégâts secondaires. Core branche depuis la cible primaire et départage distance/ID ; Headless, Viewer et RL lisent cette même voie. | partiel — projectiles logiques partagés ; sélection client, trajectoire, collision et effet visuel non mesurés | partiel — même Core | `super_wizard_tower_tests`, snapshot/replay/hash + CLI headless | non_conforme : éligibilité exacte, bords de rayon, ordre client, perte de cible et fidélité visuelle ouverts |
| Revenge Tower — données seulement | revenge_tower L1-2, Supercharge L1-2 | La [preuve initiale](../data/reference/th18-2026-09-19-revenge-tower-current-evidence.json) conserve L1-2/Supercharge et les trois modes du payload gelé. Le [suivi du 20 septembre](../data/reference/th18-2026-09-20-revenge-tower-followup.json) recoupe les seuils actuels 5/25/50, 350 ms au stade final, les cibles air/sol et les rebonds ; Supercell reste la source des seuils et cadence. Le gain exact de PV, le décompte/destruction au même tick, les transitions, sélection/branches, collision et trajectoires restent non publiés : aucune tourelle générique n’est activée. | non | non | `validate_th18_active_reference.py` + `validate_raw_catalogue_consistency.py` + registre d’incertitude | comportement_manquant_explicitement_bloqué |
| Station de fabrication — bâtiment passif L1 | crafting_station L1 | Le [baseline additif du 20 septembre](../data/reference/th18-2026-09-20-crafting-station-passive-baseline.json) préserve 1 000 PV, l’emprise 3×3, l’asset et la présence dans les snapshots de base confirmée par Supercell. La source secondaire datée la classe, avant sélection d’une défense fabriquée, parmi les bâtiments défensifs prioritaires. Le Core matérialise donc uniquement ce bâtiment passif destructible ; Headless, Viewer (géométrie/asset) et RL utilisent ce même état. Le cycle de phase, sa sélection, les modules et toute défense fabriquée temporaire restent non modélisés. | partiel — bâtiment passif, snapshot/replay, géométrie et CLI testés | partiel — même Core | `permanent_building_tests`, `viewer_selection_tests`, CLI headless, runtime RL/pybind11 | non_conforme : contrat officiel de ciblage/destruction, phase, collision et ordre au tick ouverts |
| Pièges | air_bomb, bomb, giant_bomb, giga_bomb, seeking_air_mine, skeleton_trap, spring_trap, tornado_trap | Giant Bomb L12, bomb, air_bomb, seeking_air_mine + spring_trap tracés ; autres importés | partiel | partiel | bomb, giant_bomb, air_bomb, seeking_air_mine + spring_trap | non_conforme |
| Bâtiments déjà présents | wall, town_hall, gold_mine, elixir_collector, gold_storage, elixir_storage, dark_elixir_storage, dark_elixir_drill, clan_castle, army_camp, barracks, dark_barracks, spell_factory, dark_spell_factory, laboratory, workshop, blacksmith, hero_hall, pet_house | La [delta Town Hall TH18](../data/reference/th18-2026-09-19-town-hall-th18-passive-scope.json) conserve un bâtiment destructible comptant pour les étoiles, sans arme : Supercell indique qu’il n’est plus défensif. Château de clan L1–14 : PV et emprise 3×3 source, cible destructible, snapshot et asset partagés ; garnison/sommeil défensif non activés | partiel | partiel | partiel | a_verifier |
| Bâtiments sans données combat | hero_banner | source secondaire : 2×2, non ciblable, sans PV, hors destruction, marge de déploiement 1 ; sprite secondaire épinglé | obstacle de déploiement | obstacle rendu avec asset source | obstacle exposé | Core + replay ; snapshot V7 lié au scénario ; référence | partiel / contrat de collision à vérifier |
| B.O.B’s Hut / Helper Hut — bâtiment passif L1 | bobs_hut, helper_hut | PV, emprises, niveau TH et assets sont importés du snapshot secondaire épinglé. Ces bâtiments destructibles sans arme sont matérialisés par le Core partagé, dans le scénario Viewer et les observations RL. Shift+clic Viewer sélectionne l'emprise Core et expose les statistiques lues depuis le même `GameData`. Le scénario CLI versionné les détruit via cinq commandes `T+10 ms` séquencées. | partiel — comportement de bâtiment passif, snapshot/replay, géométrie, sélection GUI et CLI headless testés ; provenance secondaire | partiel — observation par le même Core | `permanent_building_tests.cpp` + `viewer_selection_tests.cpp` + `permanent_building_headless_cli_validation` + matérialisation catalogue + runtime RL/pybind11 | a_verifier |

Le [support Supercell sur les troupes et sorts de Château de clan](https://support.supercell.com/clash-of-clans/en/articles/clan-castle-troops-and-spells-2.html), consulté le 17 septembre 2026 et épinglé comme source primaire, confirme seulement la condition générale de sortie des renforts pendant une attaque. Il ne donne pas leur alerte, composition, position de spawn, ordre de cible ni retour : le bâtiment statique est exécuté, mais toute garnison défensive demeure dans le registre de delta.
| Permanent Heroes | barbarian_king L101-110, archer_queen L101-110, grand_warden L76-85 ground and air modes, royal_champion L51-55, minion_prince L91-95, dragon_duke L21-25 | The dated, hash-pinned Hero deltas materialize all applicable TH18 level slices, source-traced base attack values, the Grand Warden air movement mode, the Royal Champion wall crossing and Dragon Duke Royal Rampage contracts, including its current 20 percent trap-damage reduction. The documented defensive targetability restrictions for Minion Prince and Dragon Duke are also shared-Core behavior. The shared Core path is exercised through Headless, Viewer and RL, with focused snapshots and value-serialized replay hash tests for every Hero. A focused regression also verifies the shared base attack when each Hero is explicitly placed as a static Core defender, plus Royal Champion defender-side Wall crossing through the same route. The four attacking-Hero-slot limit is enforced by Core. | partial: the Viewer resolves downloaded, hash-verified icon and Hero Banner variant bytes, but byte provenance is not a redistribution license. The official Fan Content Policy is not a compatible authorization to create a game based on Supercell characters. No further extracted or fan-kit art is imported. The source does not establish a battlefield sprite, scale, anchor or animation. | partial: each adapter uses the same deterministic Core state. Banner variants are explicit source-backed assignment values only, while Hero Banner defense remains unavailable | focused Hero behavior, static defender, snapshot V21 migration, replay, determinism, Headless CLI, Viewer asset validation and RL runtime tests | partial and documented: abilities including Grand Warden aura or protection and Seeking Shield are Hero Equipment and excluded. The Grand Warden source establishes air movement but not the mode-transition timing or ground-mode Wall interaction. The [2026-09-23 evidence closure](../data/reference/th18-2026-09-23-hero-defense-evidence-closure.json) marks this phase partial: official documentation establishes patrol near a Hero Banner but does not specify patrol, acquisition, return, respawn, collision or tick ordering. Public asset rights and battlefield-art fidelity also remain open. |
| Gardiens TH18 données seulement | smasher, longshot | niveaux, images et propriétés normales importés du snapshot gelé. L’annonce officielle définit leurs rôles, mais pas la géométrie/timing de patrouille, la Rage de Smasher ni l’explosion de Longshot. | non | non | audit_structure + `validate_raw_catalogue_consistency.py` | comportement_manquant |
| Logger — données seulement | logger | Artefact officiel épinglé : L1–5, 350 DPS, portée 7/7/7/7/8, cible air/sol, projectile perçant lent et recul d’une case. PV, collision, activation, projectile et déplacement restent non spécifiés. | non | non | `validate_th18_reference.py`, `validate_raw_catalogue_consistency.py`, `validate_th18_documentation_delta.py` | comportement_manquant |
| Familiers données seulement | lassi, electro_owl, mighty_yak, unicorn, diggy, poison_lizard, phoenix, frosty, spirit_fox, angry_jelly, sneezy, greedy_raven | niveaux normaux importés du snapshot historique figé ; liaison au héros, capacités, cibles et états spéciaux non activés | non | non | audit_structure | comportement_manquant |
| Engins de siège données seulement | wall_wrecker, battle_blimp, stone_slammer, siege_barracks, log_launcher, flame_flinger, battle_drill, troop_launcher | niveaux normaux importés du snapshot historique figé ; déplacement, armes, troupes libérées et destruction non activés | non | non | audit_structure | comportement_manquant |
| Sky Wagon — données seulement | sky_wagon | Artefact secondaire épinglé : L1–4, 3 600→4 200 PV, DPS 1→4, portée 5,5, cadence 8 s, coûts/prérequis et payload post-équilibrage. Les règles officielles de slot siège, immunité aux sorts et libération à la destruction sont tracées ; géométrie, barils et cycle restent ouverts. | non | non | `validate_th18_reference.py`, `validate_raw_catalogue_consistency.py`, `validate_th18_documentation_delta.py` | comportement_manquant |
| Équipements de héros — données seulement | action_figure, archer_puppet, barbarian_puppet, dark_crown, dark_orb, earthquake_boots, electro_boots, eternal_tome, fire_heart, fireball, flame_blower, frost_flake, frozen_arrow, giant_arrow, giant_gauntlet, haste_vial, healer_puppet, healing_tome, henchmen_puppet, heroic_torch, hog_rider_puppet, invisibility_vial, lavaloon_puppet, life_gem, magic_mirror, metal_pants, meteor_staff, noble_iron, rage_gem, rage_vial, rocket_backpack, rocket_spear, royal_gem, seeking_shield, snake_bracelet, spiky_ball, stick_horse, stun_blaster, vampstache | Les 39 payloads du snapshot gelé conservent les champs de chaque niveau (bonus, minerais, prérequis, capacité et héros associé) dans une voie de données qui ne les transforme pas en acteur autonome. | non | non | audit_structure + `validate_raw_catalogue_consistency.py` | comportement_manquant |

| Electro Fangs — données seulement | electro_fangs | Artefact secondaire épinglé : 18 niveaux, bonus de PV du héros, coûts d’ores et Forgeron 10 ; l’annonce officielle trace la chaîne passive (≤4 cibles, -20 %/saut, ≤400 dégâts). Les valeurs/liaisons par niveau restent ouvertes. | non | non | `validate_th18_reference.py`, `validate_raw_catalogue_consistency.py`, `validate_th18_documentation_delta.py` | comportement_manquant |
| Monolith Arrow — projection seule | monolith_arrow | Artefact secondaire épinglé : 27 niveaux, PV du héros, ores et prérequis Forgeron. Core expose le logement déployé, le palier officiel (0–180, 181–250, 251+) et le pourcentage de PV maximum correspondant (14/10/5) en lecture seule dans Headless, Viewer et RL ; snapshot, replay et hachage sont vérifiés. L’activation, l'application des dégâts, le projectile et l’ordre d’effets restent ouverts. | non | non | `cocsim_monolith_arrow_housing_tests`, `validate_hero_support_catalogue.py`, `validate_rl_runtime.py` | comportement_manquant |

The [active Hero goal scope](../data/reference/th18-2026-09-23-hero-pet-equipment-scope.json)
began with 12 permanent Pets and 41 Equipment items, including the separately
catalogued Electro Fangs and Monolith Arrow. The dated [Revenge Deck correction](../data/reference/th18-2026-09-23-revenge-deck-inventory-and-counter-audit.json)
raises the active Equipment count to 42 while preserving the frozen index. The [per-item coverage matrix](../data/reference/th18-hero-pet-equipment-coverage-2026-09-23.json)
tracks their catalogue levels, pinned raw provenance, Core and adapter status,
serialization, and open uncertainties. Catalogue presence is not combat
integration; the historical Hero-only row above describes the earlier phase.

The [L.A.S.S.I L1-15 audit](../data/reference/th18-2026-09-23-lassi-combat-contract-audit.json)
retains the official 2.5-tile Hero preference, ground targeting and Wall-jump
capability, plus the pinned older level table. Hero-linked spawning, following,
target loss, post-Hero-death behavior and Wall-jump collision lack a complete
deterministic contract. Its per-item Core, adapter, snapshot and replay axes
remain explicitly unimplemented in the matrix.

The [Electro Owl L1-15 audit](../data/reference/th18-2026-09-23-electro-owl-level-and-behavior-audit.json)
preserves pinned per-level Pet damage fields and official 2024 L11-15
hitpoints and DPS with provenance. It remains partial and catalogue-only:
current-build values, Hero-linked lifecycle, chain-target and projectile
timing, air collision, anchors and battlefield art rights remain unresolved.
The same raw-field normalization retains attack and healing values for all
12 catalogued Pets; it does not enable combat actors.
The complete post-change suite passed 157 of 157 tests.

The [Mighty Yak L1-15 audit](../data/reference/th18-2026-09-23-mighty-yak-level-and-wall-contract-audit.json)
preserves the pinned per-level Wall damage and official 2023 L11-15 hitpoints
and DPS. Supercell removed its original splash damage in 2021 and changed Wall
AI in 2026. Current movement speed, Wall selection, paired lifecycle,
post-Hero-death rage, collision and asset rights remain unresolved. The Yak
therefore remains catalogue-only, with no Core or adapter claim.
The complete post-change suite passed 157 of 157 tests.

The [Unicorn L1-15 audit](../data/reference/th18-2026-09-23-unicorn-healing-and-lifecycle-audit.json)
retains pinned per-level healing pulses. Supercell confirms the L4-10 healing
balance values and L11-15 availability, but has not published the current
numeric L11-15 values, first pulse timing or post-Hero-loss target priority.
The Unicorn remains catalogue-only with no Core or adapter claim.
The complete post-change suite passed 157 of 157 tests.

The [Frosty and Frostmite audit](../data/reference/th18-2026-09-23-frosty-frostmite-contract-audit.json)
preserves the pinned Frosty summon counts and caps and records the Frostmite
child separately in the per-item matrix. The official sources establish the
spawned child's defense targeting and slowing role, but give no current child
stats, spawn phase, slow values or stacking. The temporary Frostmite Spell is
outside this goal and supplies no permanent child values.
The complete post-change suite passed 157 of 157 tests.

The [Diggy L1-15 audit](../data/reference/th18-2026-09-23-diggy-current-level-and-behavior-audit.json)
adds a [hash-pinned L11-15 observation](../data/reference/th18-2026-09-23-diggy-l11-l15-observation.json)
from game-file data dated 2026-09-01. Supercell confirms the TH18/Pet House
12 unlock, a level-15 stun improvement, wider stun eligibility and a 2026
underground-logic rework. The exact current stun, emergence, target transition
and Hero reassignment contract remains unsourced. All Diggy levels remain
catalogue-only, and the new levels explicitly cannot materialize in Core.
The complete post-change suite passed 157 of 157 tests.

The [Poison Lizard L1-15 audit](../data/reference/th18-2026-09-23-poison-lizard-level-and-effect-audit.json)
preserves dated game-file HP and DPS, the pinned per-level poison DPS and slow
percentages, and Supercell's 4.5-tile attack range and enemy-unit target
preference. The exact enemy search radius, poison duration and stacking,
projectile timing, and paired-Hero lifecycle remain unsourced. All levels
remain catalogue-only; no Pet actor or adapter behavior is claimed.
The complete post-change suite passed 157 of 157 tests.

The [Phoenix L1-10 audit](../data/reference/th18-2026-09-23-phoenix-revival-and-interaction-audit.json)
confirms its TH18 level cap and dated HP/DPS table, and retains the pinned
six-to-eight-second revival duration by level. Supercell's egg, temporary
protection and continued Phoenix combat are recorded alongside Grand Warden
passive-aura and Dragon Duke dash interactions. Exact fatal-hit interception,
shield and knockout tick order, egg pathing, splash geometry and Revive Spell
interactions remain unsourced. Phoenix remains catalogue-only.
The complete post-change suite passed 157 of 157 tests.

The [Spirit Fox L1-10 audit](../data/reference/th18-2026-09-23-spirit-fox-invisibility-contract-audit.json)
retains Supercell's per-level Spirit Walk duration and its later six-second
visible interval. A dated game-file table confirms L10 as the TH18 maximum and
the pinned HP/DPS values. Supercell also removed frost from Fox attacks.
First activation, target loss during invisibility, paired-Hero transitions,
attack timing and visual geometry remain unsourced. Spirit Fox remains
catalogue-only.
The complete post-change suite passed 157 of 157 tests.

The [Angry Jelly L1-10 audit](../data/reference/th18-2026-09-23-angry-jelly-brainwash-contract-audit.json)
retains Supercell's per-level Brainwash durations and its attached immunity,
defense-only Hero targeting and detach condition. Two dated game-file pages
report five-tile range, superseding the pinned older payload's 1.5 tiles with
explicit catalogue provenance. Exact attachment start, defense selection,
detach order, and independent Pet attack and target timing remain unsourced.
Angry Jelly remains catalogue-only.
The complete post-change suite passed 157 of 157 tests.

The [Sneezy and Booger audit](../data/reference/th18-2026-09-23-sneezy-booger-contract-audit.json)
confirms Sneezy L1-10 HP/DPS from dated game files, preserves the pinned
two-Booger cap and rage fields as data, and adds Booger as a separate spawned
unit in the coverage matrix. Supercell says Boogers attack Buildings and no
longer trigger traps, while Sneezy attacks Defenses after her Hero falls.
Current Booger stats, spawn and cap timing, rage meaning, Grand Warden follow
weight and the 5.5-versus-2.5-tile range semantics remain unresolved. Both
actors remain catalogue-only.
The complete post-change suite passed 157 of 157 tests.

The [Greedy Raven L1-10 audit](../data/reference/th18-2026-09-23-greedy-raven-resource-target-audit.json)
confirms its TH18 level cap, dated HP/DPS and eight-tile range. The pinned
per-level resource-building DPS is retained and equals Supercell's stated
five-times damage bonus. The exact nearby-resource search radius, eligible
building set, fallback target, per-hit rounding, projectile impact and
paired-Hero transitions remain unsourced. Greedy Raven stays catalogue-only.
The complete post-change suite passed 157 of 157 tests.

The [Revenge Deck inventory and counter audit](../data/reference/th18-2026-09-23-revenge-deck-inventory-and-counter-audit.json)
adds Supercell's August 2026 Dragon Duke Epic Equipment to the active roster.
Its dated L1-27 HP boosts, ore costs and Blacksmith requirements are pinned in
a separate observation and normalized as data-only. Counter damage, any healing,
trigger eligibility, cooldown, tick order and interaction with other Equipment
remain unsourced, so no combat actor or adapter behavior is enabled. Portal
Pendant appears in one game-file index without verified release; it is outside
the current 42-item roster. The complete post-change suite passed 157 of 157
tests.

The [Barbarian Puppet L1-18 summon audit](../data/reference/th18-2026-09-23-barbarian-puppet-summon-contract-audit.json)
confirms its active Barbarian King ability, two-slot Equipment rule, Hero HP
bonuses and Raged Barbarian summon. Dated tables corroborate its HP boosts and
upgrade costs; pinned fields retain per-level summon counts, Rage values and
Hero recovery. Spawned Barbarians have their own unresolved coverage row.
Their unit level, spawn geometry, activation and Rage tick order, stat stacking
and art rights remain unknown, so the Equipment and child actor stay
catalogue-only. The complete post-change suite passed 157 of 157 tests.

The [Rage Vial L1-18 effect-order audit](../data/reference/th18-2026-09-23-rage-vial-effect-order-audit.json)
records the official active Barbarian King Rage, damage and movement bonuses,
and HP recovery. Dated tables corroborate the pinned per-level DPS and recovery
values. The source does not establish when passive DPS and active Rage combine,
how speed rounds into fixed steps, the first and final buff ticks, or recovery
and other Equipment ordering. Rage Vial remains catalogue-only in the per-item
matrix until these combat rules can be implemented faithfully.
The complete post-change suite passed 157 of 157 tests.

The [Earthquake Boots L1-18 ground-target audit](../data/reference/th18-2026-09-23-earthquake-boots-ground-target-audit.json)
records Supercell's 2025 building-damage reduction and Skeleton Trap exclusion,
then its 2026 addition of ground-troop damage. Dated tables corroborate pinned
Hero HP and DPS bonuses. Current area percentages, Wall formula, radius units,
hitbox edge inclusion and the Skeleton Trap exception after 2026 remain open.
Earthquake Boots stays catalogue-only in the per-item matrix.
The complete post-change suite passed 157 of 157 tests.

The [Vampstache L1-18 heal-event audit](../data/reference/th18-2026-09-23-vampstache-heal-event-audit.json)
records Supercell's passive heal on each Barbarian King attack and a 2025
buff. Dated tables confirm its DPS bonuses, while the pinned heal-per-hit and
attack-speed fields remain uncorroborated for the current build. Attack versus
impact trigger, Wall and immune-target eligibility, death order, heal cap and
speed-to-cadence conversion are unresolved. Vampstache remains catalogue-only.
The complete post-change suite passed 157 of 157 tests.

The [Giant Gauntlet L1-27 current-effect audit](../data/reference/th18-2026-09-23-giant-gauntlet-current-effect-audit.json)
separates Supercell's launch 2.5-tile area and level table from its January
2026 all-level duration and damage-reduction buff. Dated tables confirm the
Equipment level cap, ore costs and DPS bonuses but omit the new duration and
reduction values. Area hitboxes, regeneration ticks and damage modifier order
also remain open. Giant Gauntlet stays catalogue-only.
The complete post-change suite passed 157 of 157 tests.

The [Spiky Ball L1-27 bounce audit](../data/reference/th18-2026-09-23-spiky-ball-bounce-and-count-audit.json)
records Supercell's Building-only bounce and its later size-aware first-target
change. Every target count in the official 2024 launch table differs from the
pinned raw series. Dated 2026 public tables verify Hero DPS, HP and upgrade
costs but omit target counts and projectile damage. Bounce geometry, retarget
and impact timing remain open, so Spiky Ball stays catalogue-only.
The complete post-change suite passed 157 of 157 tests.

The [Snake Bracelet L1-27 and Snake L1-10 summon audit](../data/reference/th18-2026-09-23-snake-bracelet-spawn-contract-audit.json)
records Supercell's permanent damage-triggered King Equipment separately from
the temporary Snake Barrel. The pinned Bracelet table maps each level to a
Snake level and a maximum summoned count; a dated 2026 client-file table
confirms Hero HP, DPS and costs but omits the child and trigger fields. An
undated child table is retained only as a research lead. Damage accumulation,
total versus live cap, placement, child combat and tick order remain open.
Bracelet and child stay catalogue-only in separate partial coverage rows.
The complete post-change suite passed 157 of 157 tests.

The [Stick Horse L1-27 deployment and Wall audit](../data/reference/th18-2026-09-23-stick-horse-deployment-and-wall-audit.json)
records Supercell's limited post-deployment King Wall-hop, speed and damage
effect. The pinned table retains 12-30 second durations, movement numbers and
attack-speed percentages; a dated 2026 table corroborates Hero HP, DPS and
costs but omits the effect fields. Movement units, Wall-hop route choice,
whether hopping expires with the speed buff, cadence rounding and tick order
remain open. Stick Horse stays catalogue-only.
The complete post-change suite passed 157 of 157 tests.

The [Archer Puppet L1-18 and spawned Archer audit](../data/reference/th18-2026-09-23-archer-puppet-summon-contract-audit.json)
records Supercell's active Queen summon and HP recovery. The pinned count and
3.5-6.5 second invisibility progression matches a dated 2024 balance table;
a 2026 table confirms Queen DPS and costs but omits summon fields. The child
Archer level, first spawn tick, formation, invisibility expiry and recovery
order remain open. Parent and spawned Archer have separate partial rows and
stay catalogue-only.
The complete post-change suite passed 157 of 157 tests.

The [Invisibility Vial L1-18 damage and target audit](../data/reference/th18-2026-09-23-invisibility-vial-damage-and-target-audit.json)
records the official Queen invisibility and stronger shot, plus Supercell's
Rage exclusion for the Vial's extra damage and its exclusion from Magic Mirror
clone damage. Pinned 4.2-7.8 second and extra-shot fields remain historical;
the dated 2026 public table confirms Queen HP and costs but omits effect
values. Target loss, projectiles, damage staging and activation/expiry tick
order remain open, so Invisibility Vial stays catalogue-only.
The complete post-change suite passed 157 of 157 tests.

The [Giant Arrow L1-18 Air Defense and damage audit](../data/reference/th18-2026-09-23-giant-arrow-air-defense-damage-audit.json)
records Supercell's May 2026 2x Air Defense damage at every level and its
published new projectile damage at L9, L12, L15 and L18 (1100, 1200, 1350,
1500). These four values are exposed as sparse catalogue metadata; the older
raw damage fields remain identifiable historical observations. The other 14
post-balance values, firing direction, collision geometry, travel and impact
tick order remain open. Giant Arrow stays catalogue-only. The full suite
passed 157 of 157 tests.

The [Healer Puppet L1-18 and spawned Healer audit](../data/reference/th18-2026-09-23-healer-puppet-summon-and-healing-audit.json)
records Supercell's Queen summon, self healing and HP bonus. The pinned
1-3 Healer count, L4-8 child level and 6-35 self-heal-per-second fields match
an independent version-unknown table. A dated 2026 public table confirms
Queen HP and costs but omits those effects; ordinary Healer L4-8 data does
not prove the spawned child's complete stat or AI inheritance. Spawn
formation, healing target and first pulse, regeneration rounding and tick
order remain open. Parent and child have separate partial rows and stay
catalogue-only.

The full post-change suite passed 157 of 157 tests.

The [Frozen Arrow L1-27 slow and impact audit](../data/reference/th18-2026-09-23-frozen-arrow-slow-and-impact-audit.json)
records Supercell's passive Queen hit slow and launch duration. A dated
September 2024 balance table lowers the slow percentage at all levels; its
new values match the pinned 25-65 percent series. The dated 2026 public
table confirms Queen DPS bonuses but omits slow values. Current slow
continuity, target eligibility, movement/cadence effect, projectile impact,
refresh, stacking and expiry order remain open. Frozen Arrow stays
catalogue-only. The full suite passed 157 of 157 tests.

The [Magic Mirror L1-27 and Queen clone audit](../data/reference/th18-2026-09-23-magic-mirror-clone-and-invisibility-audit.json)
records Supercell's clone summon, one-second original-Queen invisibility
and clone exclusions from Equipment damage and passive DPS bonuses. Pinned
clone count, duration, DPS and HP agree with an independent version-unknown
table; a dated 2026 public table confirms Queen HP but omits clone effects.
Current clone values, placement, attack and expiry order, Queen target loss
and other modifier interactions remain open. Parent and clone have separate
partial rows and stay catalogue-only. The full suite passed 157 of 157 tests.

The [Action Figure L1-27 and Giant Giant audit](../data/reference/th18-2026-09-23-action-figure-giant-giant-contract-audit.json)
records the permanent Queen Equipment summon and pinned child L1-10 mapping.
A dated 2026 public table confirms Queen HP/DPS but omits child effects.
Supercell separately offers a temporary event Giant Giant; those event
troop values are not copied onto the Equipment spawn. Current child stats,
spawn and target order, Queen self healing and one-second invisibility timing
remain open. Parent and child have separate partial, catalogue-only rows.
The full suite passed 157 of 157 tests.

The [Monolith Arrow L1-27 housing and projectile audit](../data/reference/th18-2026-09-23-monolith-arrow-housing-and-projectile-audit.json)
rechecks the frozen supplemental Queen Equipment and its official deployed
housing tiers: 14 percent, 10 percent or 5 percent of target maximum HP.
The current public table confirms the 27 Queen HP bonuses. The coverage
matrix now reads the Hero assignment from supplemental records, so Monolith
Arrow is grouped with Archer Queen; the Equipment count stays 42. The
official 10 percent damage-reduction wording, attack-link trigger timing,
housing sample time, projectile impact and integer damage rounding remain
open. The brief June event boost is excluded. Monolith Arrow stays
catalogue-only. The full suite passed 157 of 157 tests.

The [dated Passive classification override](../data/reference/th18-2026-09-23-monolith-arrow-passive-type-override.json)
uses an explicit June 2026 secondary label, repeated in the same publisher's
Equipment guide. Core, Headless, Viewer and RL now expose Passive with pinned
provenance. The primary release note does not state the class, and damage,
projectile and mitigation rules remain partial. The full 164/164 suite and
separate RL runtime check passed.

The [Henchmen Puppet L1-18 and Henchman L1-7 audit](../data/reference/th18-2026-09-23-henchmen-puppet-and-child-contract-audit.json)
records its active Minion Prince summon and two flying children. Official
TH18 notes confirm a 500 HP increase for all Minion Prince Equipment; a
dated game-file table corroborates all 18 Equipment DPS bonuses. The pinned
child level map, one-second invisibility and recovery fields lack a complete
current combat contract. The community child HP/DPS table has no pinned game
version, so it remains a research lead. Child attacks, geometry, activation,
targeting, invisibility and tick order remain open; the Equipment and child
stay catalogue-only. The full suite passed 157 of 157 tests.

The [Dark Orb L1-18 audit](../data/reference/th18-2026-09-23-dark-orb-projectile-and-slow-audit.json)
records the Minion Prince's active damage-and-slow projectile. A dated
game-file table corroborates all 18 DPS bonuses and corrects the frozen L8
HP bonus from 600 to 690 through additive current metadata. The frozen payload
retains projectile damage 45-200 and slow 20-50 percent for 8-18 seconds.
The current effect values, launch direction, collision, targets, stacking,
expiry and tick ordering remain unverified. Dark Orb stays catalogue-only.
The full suite passed 157 of 157 tests.

The [Metal Pants L1-18 audit](../data/reference/th18-2026-09-23-metal-pants-barrier-and-recovery-audit.json)
records official active Minion Prince protection and the TH18 Equipment
HP adjustment. A dated game-file table corroborates all 18 Prince HP
bonuses. The pinned 9-15 second duration, 46-70 percent damage reduction
and HP recovery series are historical fields without current effect
confirmation. Damage-origin eligibility, mitigation order, activation,
expiry, recovery and modifier stacking remain open. Metal Pants stays
catalogue-only. The full suite passed 157 of 157 tests.

The [Noble Iron L1-18 opening-shot audit](../data/reference/th18-2026-09-23-noble-iron-opening-shots-audit.json)
records the official passive Minion Prince boost to his first projectiles
and the January 2026 all-level range, count and attack-speed buffs. The
dated table confirms +500 Prince HP at every level. The pinned post-buff
candidate damage, range, count and speed values lack a versioned current
effect table. Charge consumption, target acquisition, projectile impact,
cadence stacking and defensive use remain open. Noble Iron stays
catalogue-only. The full suite passed 157 of 157 tests.

The [Dark Crown L1-27 defeat-threshold audit](../data/reference/th18-2026-09-23-dark-crown-defeat-threshold-audit.json)
records Supercell's three Prince HP and power boosts after friendly Troop
defeats. A dated launch report gives historical cumulative thresholds of
60, 120 and 180 defeated housing space with exclusions; the dated 2026
table confirms Prince HP bonuses, but omits effect values. Eligibility for
Heroes, Pets and spawned children, per-stage versus total HP growth,
current-HP fill, attack-speed stacking and simultaneous death order remain
open. Dark Crown stays catalogue-only. The full suite passed 157 of 157 tests.

The [Meteor Staff L1-27 target and cadence audit](../data/reference/th18-2026-09-23-meteor-staff-target-and-cadence-audit.json)
records official repeated strikes against the nearest Defense. The dated
2026 table confirms Prince HP/DPS bonuses; the pinned 5-10 second cooldown
and 250-850 strike damage remain historical effect values. First-strike
timing, nearest-target geometry, projectile travel, impact and retargeting
are open. The separate Cosmic Rock battlefield meteors and temporary
Meteor Golem do not supply Equipment behavior. Meteor Staff stays
catalogue-only. The full suite passed 157 of 157 tests.

The [Eternal Tome L1-18 aura-immunity audit](../data/reference/th18-2026-09-23-eternal-tome-aura-immunity-audit.json)
records the official June 2025 change: Troops entering the Warden aura after
activation gain damage immunity, and leaving Troops retain it for one second.
The pinned 3.2-8.2 second duration series matches a dated post-change table;
the +0.2 second balance change must not be applied again. Aura geometry,
eligible classes, boundary behavior, auto-activation and same-tick damage
ordering remain open. Eternal Tome stays catalogue-only.
The full suite passed 157 of 157 tests.

The [Life Gem L1-18 balance and aura audit](../data/reference/th18-2026-09-23-life-gem-balance-and-aura-audit.json)
records official January 2026 increases to L1-7 extra HP/caps and L1-18
Warden HP/DPS. Frozen raw values and a dated 2026 table do not give complete
current replacements. Official later notes exclude Totems and siege machines
from the aura. Geometry, eligible in-scope classes, current-HP conversion,
stacking and tick order remain open. Life Gem stays catalogue-only.
The full suite passed 157 of 157 tests.

The [Rage Gem L1-18 aura and stacking audit](../data/reference/th18-2026-09-23-rage-gem-aura-and-stacking-audit.json)
records the official passive nearby-unit damage aura and separate Warden
DPS/attack-speed bonuses. A dated 2026 table corroborates Warden DPS and
costs; pinned 15-50 percent aura values remain unconfirmed as current.
Aura geometry, eligible classes, self-inclusion, damage staging, stacking and
tick order remain open. Rage Gem stays catalogue-only.
The full suite passed 157 of 157 tests.

The [Healing Tome L1-18 aura and pulse audit](../data/reference/th18-2026-09-23-healing-tome-aura-and-pulse-audit.json)
records official active Warden and nearby-unit healing. The pinned L1-18
60-150 healing-per-second and 15-20-second durations match historical 2024
duration reductions, but current effect continuity and the actual heal pulse
are unverified. Aura geometry, eligible classes, Warden recovery, rounding
and healing/damage tick order remain open. Healing Tome stays catalogue-only.
The full suite passed 157 of 157 tests.

The [Fireball L1-27 target and splash audit](../data/reference/th18-2026-09-23-fireball-target-size-and-splash-audit.json)
records official active nearest-Defense area damage, the June 2024 change to
building-size-aware initial targeting and ground/air splash. The launch L18
radius is obsolete after the 2025 change; the pinned series gives 6 tiles
from L24. Exact distance, impact timing, splash geometry, eligibility and
current effect continuity remain open. Fireball stays catalogue-only.
The full suite passed 157 of 157 tests.

The [Lavaloon Puppet L1-27 and child audit](../data/reference/th18-2026-09-23-lavaloon-puppet-and-child-contract-audit.json)
records the official L21-27 increase to three summoned Lavaloons and the
8.7% housing reduction. Pinned counts are 1/2/3 across L1-8/L9-20/L21-27;
a secondary report gives current housing 21. The spawned Lavaloon lacks a
current combat table, target and placement contract. Lavaloon Pup housing is
officially one, while its relationship to the permanent Equipment variant
is unverified. Equipment and children stay catalogue-only. The full suite
passed 157 of 157 tests.

The [Heroic Torch L1-27 balance and Wall audit](../data/reference/th18-2026-09-23-heroic-torch-balance-and-wall-audit.json)
records official nearby Troop Wall pass-through, speed and damage reduction,
the later Warden speed inclusion and Totem exclusion. January 2026 buffs
supersede several pinned fields without a replacement level table. Aura
geometry, eligible classes, Wall exit, current values and tick order remain
open. Heroic Torch stays catalogue-only. The full suite passed 157 of 157 tests.

The [Royal Gem L1-18 recovery and bonus audit](../data/reference/th18-2026-09-23-royal-gem-recovery-and-bonus-audit.json)
records official Royal Champion self-healing and HP/DPS bonuses. Dated
sources match pinned HP 60-570 and DPS 35-120, while the current active heal,
cap, activation phase and max/current HP conversion remain open. Royal Gem
stays catalogue-only. The full suite passed 157 of 157 tests.

The [Seeking Shield L1-18 target and bounce audit](../data/reference/th18-2026-09-23-seeking-shield-target-and-bounce-audit.json)
records official four-Defense targeting without a range limit, defensive
Builder exclusion and size-aware initial selection. The current projectile
damage, footprint metric, bounce selection and impact timing remain open.
Seeking Shield stays catalogue-only. The full suite passed 157 of 157 tests.

The [Hog Rider Puppet L1-18 and child audit](../data/reference/th18-2026-09-23-hog-rider-puppet-and-child-audit.json)
records the official Royal Champion summon and January 2026 L1-7 child-count
and level buffs. Percentage ranges do not identify exact current values by
level. Child stat inheritance, formation, spawn tick and Champion recovery/
invisibility order remain open. Equipment and child stay catalogue-only. The
full suite passed 157 of 157 tests.

The [Haste Vial L1-18 cadence and speed audit](../data/reference/th18-2026-09-23-haste-vial-cadence-and-speed-audit.json)
records official Royal Champion movement and attack-speed boosts. The April
2024 L9-18 passive attack-speed changes match pinned values. Current active
values, movement units, integer tick cadence conversion, stacking and expiry
order remain open. Haste Vial stays catalogue-only. The full suite passed
157 of 157 tests.

The [Rocket Spear L1-27 charged shots audit](../data/reference/th18-2026-09-23-rocket-spear-charged-shots-audit.json)
records the official Royal Champion active ability and pinned level-dependent
shot count, extra damage, range and splash labels. Current values, projectile
geometry, charge consumption, splash eligibility and fixed-tick activation/
impact order remain open. Rocket Spear stays catalogue-only. The full suite
passed 157 of 157 tests.

The [Electro Boots L1-27 aura and balance audit](../data/reference/th18-2026-09-23-electro-boots-aura-and-balance-audit.json)
records the official Royal Champion periodic aura and later nerf, with pinned
400 ms pulse, 5 tile radius and damage/healing series. Current post-nerf
values, target geometry, fractional arithmetic and tick order remain open.
Electro Boots stays catalogue-only. The full suite passed 157 of 157 tests.

The [Frost Flake L1-27 freeze and target audit](../data/reference/th18-2026-09-23-frost-flake-freeze-and-target-audit.json)
records its permanent Royal Champion assignment and pinned projectile damage,
target count, freeze duration and slowdown label. Current values, target
fallback and geometry, projectile impact and freeze tick order remain open.
Frost Flake stays catalogue-only. The corrected full suite passed 157 of 157
tests.

The [Fire Heart L1-18 balance and death burst audit](../data/reference/th18-2026-09-23-fire-heart-balance-and-death-burst-audit.json)
records the official May 2026 replacement DPS series and L15/L18 regeneration
values separately from the frozen raw values. Current L16/L17 regeneration,
death burst damage and geometry, integer healing accrual and death tick order
remain open. Fire Heart stays catalogue-only. The full suite passed 157 of 157
tests.

The [Flame Blower L1-18 direction and activation audit](../data/reference/th18-2026-09-23-flame-blower-direction-and-activation-audit.json)
records official fixed activation before Rocket Backpack and the August 2026
facing-direction fix. Pinned L15 damage exceeds L16/L17 without a correction
source. Current damage, flame geometry, facing sample and hit order remain
open. Flame Blower stays catalogue-only. The full suite passed 157 of 157
tests.

The [Stun Blaster L1-18 shockwave and target audit](../data/reference/th18-2026-09-23-stun-blaster-shockwave-and-target-audit.json)
records pinned damage, stun duration and radius labels alongside dated Duke
HP/DPS bonuses. Community reports of multiple pulses and target exclusions
lack a complete timing, target and recovery contract. Stun Blaster stays
catalogue-only. The full suite passed 157 of 157 tests.

The [Electro Fangs L1-18 chain and target audit](../data/reference/th18-2026-09-23-electro-fangs-chain-and-target-audit.json)
records official four-target chaining, 20% decay and 400 maximum strike
damage alongside the frozen and dated HP/ore table. Community per-level
damage/counts lack primary corroboration; link radius, target choice and
integer rounding remain open. Electro Fangs stays catalogue-only. The full
suite passed 157 of 157 tests.

The [Rocket Backpack L1-27 dash and balance audit](../data/reference/th18-2026-09-23-rocket-backpack-dash-and-balance-audit.json)
records official L21/L27 penetrating damage replacements, fixed Flame Blower
activation order and August 2026 dash/Phoenix fixes. Intermediate damage,
dash path, collision, area geometry and fixed-tick lifecycle remain open.
Rocket Backpack stays catalogue-only. The full suite passed 157 of 157 tests.

## Deltas de comportement connus

### Audit TH18 — incrément provenance et placement

Le catalogue normalisé V3 porte maintenant une provenance par niveau pour les
contenus **et les sorts** : URL brute, commit immuable, SHA-256 et date de
consultation. Les surcharges sont lues depuis le registre TH18 versionné, et non
depuis une constante opaque du code. `validate_th18_reference.py` vérifie le
correctif officiel Dragon Rider L6 et la provenance de Séisme L8.

Le même validateur contrôle désormais tout le registre de sources (éditeur,
type primaire/secondaire/expérimental, URL HTTPS et date ISO de consultation)
ainsi que le hash SHA-256 et le type de source de chaque provenance de niveau.
Une entrée de catalogue ne peut donc pas acquérir silencieusement une valeur
sans piste de reproduction dans la référence gelée. Les bornes d’énumération
des troupes et des sorts sont aussi explicites : un sentinel ou une valeur
future est refusé au chargement de snapshot plutôt que d’être transformé en
commande/replay non nommable. Les régressions Core couvrent ce rejet ; les
validateurs Viewer et RL relisent la table de noms sans dépendre de la forme
interne de cette garde.

Le Viewer ne recopie plus la règle de déploiement : ses cadres rouges ne sont
qu'une aide visuelle et chaque clic passe par `BattleState::submit`/`deploy`,
comme les commandes RL et replay. La régression C++ vérifie le refus dans la
marge d'une tuile d'un Canon importé 3×3, ainsi que l'acceptation exacte à la
frontière extérieure. Cette vérification ne clôt pas les autres tailles : elles
restent `a_verifier` jusqu'à rapprochement avec une source TH18 correspondante.
Les appels GUI/RL sont maintenant datés explicitement au prochain tick (`T+10 ms`)
et le core refuse une commande au tick courant ou dans le passé ; une régression
protège cette propriété de déterminisme. Le format de replay écrit est V3 ; le
lecteur migre explicitement le `T+0 ms` des journaux V1/V2 vers `T+10 ms` afin de
préserver leur première action sous cette règle, avec une régression dédiée.

Le registre confirme aussi que les niveaux annoncés officiellement sont chargés
pour Mortier 18, Inferno 12, Tesla cachée 17, Archer 14, Chevaucheur de cochon
15, Mur 19, Maison des familiers 12 et Séisme 6–8. Le validateur contrôle cette
présence à chaque exécution. Leurs chiffres restent `secondary_unverified`, sauf
les PV/DPS du Dragon Rider 6 : une présence de niveau n’est jamais confondue
avec une fidélité numérique ou comportementale.

Le Sapeur a désormais son multiplicateur ×40 contre les murs, son blast d'attaque
de rayon deux et son auto-destruction après une attaque réussie, tous portés par
le catalogue versionné et testés via le même core que le Viewer et RL. Il ne
reçoit pas un second blast de mort : la provenance associée à
`self_destruct_on_attack` le documente explicitement. La sélection exacte d'un
mur « protégeant » une structure (topologie de compartiment) reste ouverte : le
moteur choisit actuellement le mur ciblable le plus proche.

Son tableau complet L1–14 est aussi rapproché d’une observation secondaire
datée du build client 18.600.3 : PV, DPS, niveau d’Hôtel de ville, logement,
cible, cadence 1 s, portée et vitesse sont vérifiés contre le catalogue. Cette
corroboration ne rend pas pour autant le pathfinding de compartiment conforme :
la règle publique ne donne toujours pas la topologie exacte du mur choisi.

Le Canon est maintenant rapproché intégralement d’une table secondaire datée du
build client 18.400.21 : niveaux L1–21, PV, DPS, niveau d’Hôtel de ville,
emprise 3×3, cible sol, portée 9 et disponibilité de sept Canons au TH18. La
table signale TH2 pour le Canon L2, contrairement au snapshot structuré d’avril
(TH1) : cette divergence est une surcharge versionnée dans la référence et une
révision append-only, contrôlées par les validateurs de référence, catalogue,
Core et RL. La table est antérieure au build gelé et ne mesure ni le timing de
projectile au tick ni les égalités de ciblage : ces lacunes restent ouvertes
dans le registre de delta, donc la fidélité du Canon demeure non conforme.

La Tour d’archers est, elle aussi, rapprochée intégralement d’une table
secondaire datée du build client 18.400.21 : niveaux L1–21, PV, DPS, niveau
d’Hôtel de ville, emprise 3×3, cible air/sol, portée 10 et neuf exemplaires au
TH18. Les nombres concordent avec le catalogue gelé ; la régression Core vérifie
également son L21. La source ne couvre ni timing de projectile au tick, ni
égalité de ciblage, ni une table associée à un build plus récent : ces limites
restent dans le delta et la Tour d’archers n’est pas déclarée fidèle.

Le tableau L1–18 du Mortier (PV, DPS, niveau d’Hôtel de ville, emprise 3×3,
portée maximale 11, cible sol, zone d’effet et quatre exemplaires TH18) concorde
avec une source secondaire datée du build 18.400.21 et sa régression Core couvre
le L18. L’audit relève toutefois une divergence qualitative : la source décrit
une zone morte autour du Mortier. Une seconde source publique documente la
portée 4–11 : `min_range=4` est donc une surcharge gelée et le Core vérifie le
refus à l’intérieur de la zone, ainsi que l’acceptation exacte à quatre tuiles.
Cette source n’identifie toutefois pas de build client ; le comportement est
partagé par Core/Viewer/RL mais reste secondaire et non vérifié contre la
version gelée jusqu’à une preuve versionnée et des replays de frontière.

La Défense aérienne est rapprochée de sa table normale L1–16 (PV, DPS, niveau
d’Hôtel de ville, emprise 3×3, portée 10 et quatre exemplaires TH18) issue du
build secondaire 18.400.21. Le Core vérifie également son contrat exclusif :
elle ignore un Barbare terrestre et attaque un Guérisseur volant ; GUI et RL
emploient la même donnée `target_type=air`. Sa variante Supercharge reste issue
du snapshot brut antérieur, sans preuve de disponibilité ou comportement TH18 :
elle figure explicitement dans le delta et ne compte pas comme fidèle.

La Tour de sorcier est rapprochée de son tableau normal L1–17 (PV, DPS, niveau
d’Hôtel de ville, emprise 3×3, portée 7, cible air/sol et six exemplaires TH18)
daté du build secondaire 18.400.21. Une régression Core confirme que l’impact
de splash atteint deux Barbares groupés via la même logique que GUI/RL. Le rayon
numérique et la géométrie exacte de la zone ne sont pas publiés pour ce build :
la frontière historique d’une tuile reste documentée comme non vérifiée, non
comme un comportement fidèle.

La Tour à bombes est rapprochée de sa table actuelle L1–13 et de l’explosion à
la destruction : deux sources secondaires datées donnent 150→700 dégâts, un
délai d’une seconde, un rayon de 2,75 tuiles et une cible sol uniquement. Le
Core conserve cet effet jusqu’au T+ms d’impact ; le Viewer affiche sa zone
orange depuis cette projection en lecture seule et RL reçoit les mêmes champs
`death_explosions`. L’ordre exact au tick avec les projectiles, sorts, Gel et
fin de bataille reste documenté comme incertitude, donc ce comportement reste
`partiel` / `a_verifier` plutôt que fidèle.

Le contrôle `validate_viewer_assets.py` exécuté sur cette référence confirme
28 niveaux rendus par un tier de fallback catalogue et quatre lacunes visuelles
source pour les deux variantes de squelettes invoqués du Piège à squelettes.
Ces fallbacks ne sont pas assimilés à des assets fidèles ; le registre de delta
des assets reste le critère bloquant pour la conformité GUI.

La Tesla cachée conserve et sérialise correctement sa révélation de proximité.
Une source secondaire ajoute le seuil de 51 % : le Core révèle alors toutes les
Tesla cachées restantes à partir de son pourcentage logique de destruction, et
GUI/RL observent le même état. Les régressions couvrent 50/101 (pas de
révélation), 52/101 (révélation) et snapshot/restauration. La pondération exacte
des bâtiments et l’ordre simultané proximité/seuil ne sont pas publiés ; cette
règle reste donc secondaire et `a_verifier`.

L’Archer L1 est corrigé à TH2 par une révision append-only après confrontation
à une table secondaire datée du build 18.400.21 ; L1–14, logement 1, cadence
1 s, portée 3,5 et cible air/sol concordent avec le catalogue. La régression
Core verrouille le niveau requis L1 et les valeurs L14. La capacité générale de
tirer au-delà d’un mur est documentée, mais la géométrie exacte de frontière et
de projectile par rapport aux murs reste incluse dans le delta commun de
pathfinding/projectiles : aucune fidélité complète n’est revendiquée.

Le Géant bénéficie du même contrôle L1–14 sur la table datée du build 18.600.3
(PV, DPS, TH, logement, cible, cadence et portée). Le core est couvert pour sa
préférence de défenses puis son repli sur une cible normale ; les mesures de
départage et de parcours au tick restent ouvertes.

Les tables actuelles L1–13 du Ballon et L1–14 du Sorcier sont également
épinglées au build 18.600.3. Elles vérifient leurs PV/DPS/TH et attributs
racines ; le Ballon conserve sa préférence de défenses et le Sorcier son splash
à distance. Les timings de projectile, départages et limites géométriques
restent des écarts documentaires, pas des comportements déclarés fidèles.

La qualification « dégâts de zone » est lue depuis `damage_type` dans le
catalogue normalisé, plutôt que depuis une liste de `Kind` du C++. Le validateur
`damage_classification_validation` protège les quatorze contenus de zone chargés
contre une divergence entre données et moteur.

De la même façon, le chemin projectile offensif est choisi par la portée niveau
(> 1 tuile) chargée depuis le catalogue, après lecture de l'arme imbriquée de
l'Hôtel de Ville. La portée de la Guérisseuse reste une portée de soin et elle
ne devient pas `ranged`, mais son effet non documenté comme immédiat suit aussi
un projectile logique `T+10 ms`. `ranged_classification_validation` et la
validation RL contrôlent cette dérivation sur les quinze contenus concernés.

Les squelettes aériens et terrestres invoqués par le Piège à squelettes sont
désormais testés jusqu'à leur première attaque. Le core déplace toute entité
mobile selon sa vitesse catalogue, y compris lorsqu'elle appartient au défenseur
(les défenses statiques restent immobiles car leur vitesse est nulle). Les seize
bâtiments passifs déjà chargés sont aussi testés comme cibles : PV et empreinte
catalogue survivent au scénario et une troupe peut les acquérir et les frapper.
Cette couverture de régression porte le total à 52/197 ; elle ne constitue pas
une certification de fidélité des règles non publiées.

L'audit refuse désormais une entrée déclarée `core_behavior` si son `Kind` ne
figure pas dans `tests/core_tests.cpp`. La matrice ne peut donc pas compter une
couverture purement déclarative ; le passage du test reste distinct de la
fidélité vérifiée.

`th18_rl_runtime_validation` compare désormais chaque niveau et variante
chargés à `GameData` via le binding : PV, dégâts, DPS, soin, portée, vitesse,
empreinte, cible, logement, rayon, multiplicateurs **et** cadence quantifiée au
tick, statut soin et dégâts de zone. Les 707 scénarios de niveau sont ensuite
instanciés par l'adaptateur Python commun.

Le déploiement RL ne fabrique plus un niveau 1 : il sélectionne le premier
emplacement de réserve non vide du même `Kind`, puis transmet son niveau au
même `Command` futur que le Viewer et le replay. La disponibilité agrège les
emplacements homonymes sans conserver de pointeur vers une chaîne temporaire.
La régression runtime déploie explicitement un Barbare niveau 13 (310 PV).
Une attaque ne termine plus non plus à `T+0` lorsqu'il ne reste que des sorts :
le core garde les réserves de sorts dans son critère terminal et un test C++
couvre le cast de Foudre sans troupe.

La Giga bombe L1–4 est maintenant une entité cachée du core : elle attend le
seuil importé de 18 logements, cible sol et air dans son rayon de déclenchement,
puis applique ses dégâts importés à toutes les troupes dans son rayon. Son
placement est visible dans les scénarios du Viewer et de RL, avec son asset
catalogue ; une régression couvre le niveau 4, le seuil et les deux altitudes.

Le Piège Tornade L1–3 est chargé depuis le catalogue et actif dans les trois
adaptateurs : il déclenche sur sol et air, maintient la durée importée et répartit
exactement le total de dégâts importé sur `kTickMs`. La note Supercell décrit le
vortex qui entrave les troupes, mais ne donne pas de trajectoire ni de force
déterministes ; le déplacement reste donc explicitement `non_conforme` plutôt
que simulé par approximation.

L'import V3 préserve les statistiques de mode par niveau. Inferno 12 contient
ses valeurs `normal` (DPS initial, palier 1,5 s et DPS final) et `multiTarget`
(dégâts, DPS, six cibles), directement extraites du snapshot secondaire haché.
Le moteur exécute le mode `multi_target` : portée 10, dégâts par rayon et plafond
de six cibles, avec sélection déterministe distance/id ; le mode est porté par
`Placement.mode`, les scénarios et les replays. Pour le rayon mono, la source
secondaire documente les paliers immédiat, 1,5 s et 5,25 s ainsi que le reset à
la perte du verrou. Le Core applique ces trois dégâts, persiste cible/durée de
verrou dans Snapshot V11 et les régressions couvrent les trois paliers, le reset
et la restauration. L'ordre exact au tick limite, le gel et les munitions restent
explicitement ouverts faute de chronologie de build publiquement vérifiable.

Les modes normalisés conservent également cadence, nombre de tirs et pause de
rafale. Le Canon `geared_up_burst` est maintenant exécutable L7+ : portée 7,
quatre projectiles à 0,192 s puis pause 1,6 s, avec les dégâts par niveau du
payload immuable. Une régression vérifie ses quatre tirs, le snapshot et le
refus explicite d'un niveau sans statistiques de mode. Le Mortier
`geared_up_burst` est maintenant exécutable à partir du niveau
8 via `Placement.mode` : la première rafale attend 0,5 s (correctif officiel
d’avril 2026), puis trois projectiles de 37,33 au niveau 8 sont espacés de
0,5 s, suivis d’une pause de 3,5 s. Les scénarios, replays, Viewer et RL passent par
la même entité core ; une régression couvre les trois tirs. Le mode rapide de
la Tour d’archers reste `manquant` : le payload racine décrit cadence/portée,
mais les dégâts par niveau ne sont pas présents, donc ils ne sont pas devinés
et le Core rejette explicitement ce mode au lieu de l'exécuter en mode normal.
La note Supercell du 20 janvier 2021 confirme seulement qu'une coordonnée
initiale de projectile a dû être corrigée pour ce mode :
<https://supercell.com/en/games/clashofclans/blog/game-updates/upcoming-balance-changes/>.
Elle ne donne ni la coordonnée, ni les dégâts, ni la vitesse de vol ; ces
lacunes restent donc ouvertes dans le delta TH18.

L'adaptateur RL expose `reset(options={"scenario": ...})`, dont les défenseurs
acceptent le champ `mode` (notamment `multi_target` et `geared_up_burst`). La
grille d'action est maintenant paramétrable avec `CoCSimEnv(width=..., height=...)`
et doit correspondre exactement au scénario : une arène de village n'est donc plus
forcée dans une grille 20x20. L'observation V0 est aussi dimensionnée par
`max_entities` (128 par défaut) et échoue explicitement au lieu de perdre des
entités ; le scénario doit choisir une capacité suffisante. Il ne reproduit aucune
règle et construit seulement `Scenario` pour `cocsim_core`.
La syntaxe Python, le core et le module pybind11 sont compilés et testés dans
l'environnement courant par `th18_rl_runtime_validation`.

Les éléments présents ne sont pas clos : la majorité utilise encore le déplacement,
la cible et le projectile génériques. Exemples explicitement ouverts : rebonds du
Bowler (à venir), munitions et gel de l'Inferno, armes Giga de Town Hall, défenses
à munitions, pièges, invocation, enfouissement, invisibilité, recall et tous les
équipements. Les sept sorts disponibles sont interactifs : les régressions Core
couvrent les dégâts instantanés de Foudre, la réduction de dégâts de Séisme,
le multiplicateur de Rage, le gel d'une défense et le snapshot d'Hâte. La
validation runtime RL exécute les sept actions et vérifie leur consommation de
réserve par le même Core que le Viewer. Cela ne ferme pas les interactions fines
de Foudre et Soin, qui restent des modèles de zone à vérifier niveau par niveau.

À TH18, l'Hôtel de Ville reste destructible mais n'est plus une défense : le core
ne lui attribue aucune attaque, et une régression le vérifie. Les armes Giga des
niveaux antérieurs ne sont donc pas extrapolées au TH18.

Hâte est importé avec son rayon **par niveau** (4 aux niveaux 1–6, 5 au niveau
7), et non comme une constante globale : une régression sur ce point est couverte
par `cocsim_tests`. Saut utilise son rayon 3,5 et sa durée propre à chaque niveau.
Le multiplicateur de dégâts du Gobelin contre les ressources est désormais
importé à ×2 depuis une table secondaire datée : il est appliqué par le
catalogue immuable aux catégories Ressources et Hôtel de Ville, avec une
régression contre une Mine d'or. Cette provenance secondaire et le repli après
épuisement de ces cibles restent à vérifier ; la valeur ne doit pas être
présentée comme officiellement validée. La Défense aérienne possède un test
ciblé confirmant qu'elle ignore une cible au sol et sélectionne une cible
volante.

La X-Bow est importée du même commit, avec sa taille 4×4, tous ses niveaux et
ses deux modes source : `normal` (sol, portée 14) et `air_and_ground` (air+sol,
portée 11,5). Le second mode est porté par `Placement.mode`, est sérialisé dans
les scénarios/replays et a une régression de ciblage aérien. Son rechargement en
élixir et les états « depleted » restent explicitement hors du modèle : cette
ligne demeure donc `partiel` / `a_verifier`.

La [référence additive du 19 septembre](../data/reference/th18-2026-09-19-valkyrie-baseline.json)
valide la Valkyrie L1–12 comme parent sol à dégâts de zone dans le Core partagé,
Headless, Viewer et RL. La table actuelle build 18.600.3, la cadence 1,8 s, le
logement 8 et l'attaque au sol sont recoupés ; le rayon sol d'une tuile est
corroboré par une source secondaire distincte. La régression L12 vérifie la
zone, snapshot, replay, hash et le scénario CLI. La valeur historique officielle
L11 (2 800 PV/240 DPS) diverge de la table actuelle (2 600/238) et reste tracée
sans réécriture ; choix de route/cible, collision, égalités, timing et visuels
restent `partiel` / `a_verifier`.

La [référence additive Dragon Rider du 19 septembre](../data/reference/th18-2026-09-19-dragon-rider-baseline.json)
rend son parent L1–6 volant, à priorité Défenses et cible sol/air disponible via
le même `GameData` immuable dans Core, Headless, Viewer et RL. Le test focalisé
couvre priorité défense, snapshot, replay, hash et scénario CLI. Le niveau 6
conserve les valeurs primaires de mai (6 000 PV, 510 DPS, 612/coup) : Supercell
a ensuite annoncé une réduction sans chiffres et la table secondaire build
18.600.3 (5 900/490) contredit aussi la portée corroborée (3,5 contre 4 tuiles).
Cette observation est donc conservée dans le delta, mais non appliquée. Les
dégâts/rayon de mort, trajectoire, collisions, égalités, ordre au tick, portée
et visuels restent `partiel` / `a_verifier`.

La [référence additive Miner du 19 septembre](../data/reference/th18-2026-09-19-miner-baseline.json)
rend son parent L1–12 et l'état souterrain existant disponibles via Core,
Headless, Viewer et RL. Le test focalisé vérifie passage des murs, non-déclenchement
de bombe pendant le tunnel, snapshot/replay/hash et scénario CLI. La durée de
tunnel/surface, les interactions détaillées et le conflit de portée (0,5/0,6)
restent `partiel` / `a_verifier`.

La [revalidation additive Electro Dragon du 19 septembre](../data/reference/th18-2026-09-19-electro-dragon-current-baseline.json)
confirme le parent L1–9 courant (build 18.600.3 rapporté), espace 30, cadence
3,5 s et portée 2,5, sans modifier les règles Core. Son éclair en chaîne partagé
par Core, Headless, Viewer et RL conserve cinq victimes sol/air distinctes et
une décroissance documentée, avec tests focalisés, snapshot, replay, hash et
scénario CLI. La métrique de saut, l'ordre précis des victimes, vol/collision,
perte de cible, ordre intra-tick et l'éclair de mort restent `partiel` / `a_verifier`.

La Bomb Tower est intégrée avec ses 13 niveaux, son empreinte 3×3, son attaque
au sol de portée 6 et son projectile de zone de rayon 1,5 tuile. Une régression
vérifie qu'un projectile touche deux troupes adjacentes. Deux sources secondaires
datées donnent les dégâts de destruction L1–13 (150→700), un délai de 1 s, un
rayon de 2,75 tuiles et le filtre sol uniquement. Le Core programme donc cette
explosion, la conserve dans Snapshot V11 et le GUI/RL l’observent via la même
simulation ; le GUI la rend comme zone orange en lecture seule et RL expose son
T+ms d’impact ainsi que, dans le catalogue, délai et filtre sol/air. La régression couvre délai, filtre air/sol et restauration. L’ordre
au tick avec projectile, sort, Gel et victoire reste ouvert, donc fidélité
`partiel` / `a_verifier`.

L'audit des pièges relève un écart de version pour la Bombe aérienne. La table
officielle de mai 2026 publie une portée d'attaque passée de 900 à 1 100 et
des cadences modifiées aux niveaux 1–10 ; le snapshot historique ne contient
que le rayon de déclenchement de 4 tuiles. Ces notions ne sont pas définies
comme équivalentes par la publication : le moteur conserve donc son activation
historique immédiate, commune au GUI et à RL, mais la matrice la classe
explicitement non fidèle tant qu'un replay ou des données client versionnées ne
séparent pas détection, lancement, projectile et impact.

La Tesla cachée a ses 17 niveaux, son empreinte 2×2, portée 7 et rayon de
déclenchement 6 importés. Avant ce déclenchement elle est absente du viewer et
des observations RL, ne peut pas être sélectionnée comme cible, puis est révélée
de manière déterministe au passage d'une troupe. Le snapshot est devenu
`COCSIM-SNAPSHOT-11` afin de conserver cet état, le compteur de Séisme, les
modes et le verrou mono-cible de l'Inferno, les explosions différées, ainsi que
l'empreinte du scénario statique ; le lecteur accepte encore V2 à V10.
La régression couvre l’état caché, snapshot/restauration et révélation. Les
interactions particulières avec les sorts restent à vérifier, donc `partiel` /
`a_verifier`.

L'audit a également écarté une règle historique fréquemment confondue avec le
jeu actuel : la Tesla cachée ne reçoit pas de multiplicateur contre P.E.K.K.A.
La documentation communautaire trace l'ancien x2, puis sa suppression en
octobre 2016 ; aucune note officielle TH18 consultée ne le réintroduit. Le
chemin de dégâts générique partagé par Core, GUI et RL est donc intentionnel,
sans prétendre remplacer une spécification client complète.

Le [contrat P.E.K.K.A additif](../data/reference/th18-2026-09-19-pekka-baseline.json)
rend ce parent L1–13 explicitement disponible via Core, CLI, GUI et RL avec
test focalisé, snapshot, replay, hash déterministe et fixture CLI. Il ne
réintroduit pas le multiplicateur Tesla historique. Les données actuelles
versionnées, les limites de cible, le trajet, la collision, les interactions
et la fidélité visuelle restent partiels.

Le [contrat Gobelin additif](../data/reference/th18-2026-09-19-goblin-baseline.json)
rend L1–10 disponible par le même chemin Core, CLI, Viewer et RL. La priorité
Ressources et le multiplicateur de dégâts ×2 sont couverts par test focalisé,
snapshot, replay, hash et fixture CLI ; le choix entre cibles égales, le
cheminement, les collisions, les interactions et la fidélité visuelle restent
explicitement partiels.

Le [contrat Minion additif](../data/reference/th18-2026-09-19-minion-baseline.json)
rend L1–14 disponible par le même chemin Core, CLI, Viewer et RL. L’exclusion
de la Mine chercheuse est une donnée immuable appliquée par le filtre de piège
du Core et couverte par test focalisé, snapshot, replay, hash et fixture CLI.
La portée publiée (2,25 cases) contredit le snapshot épinglé (2,75), donc le
moteur ne prétend pas vérifier cette géométrie ; projectile, vol, collisions,
ties, interactions restantes et rendu demeurent partiels.

Le [contrat Hog Rider additif](../data/reference/th18-2026-09-19-hog-rider-baseline.json)
rend L1–15 disponible par le même chemin Core, CLI, Viewer et RL. Il fixe la
priorité Défenses et exclut les Murs de la grille de chemin du Core ; le test
vérifie cette priorité, le passage du Mur, snapshot, replay, hash et fixture
CLI. Le choix de route, la durée et la trajectoire du saut, les collisions,
ties, interactions et rendu restent explicitement partiels.

Le Bébé Dragon L1–12 est désormais importé du snapshot gelé et déployable par
le catalogue commun, avec le niveau 12 TH18 corroboré par la note officielle.
Le [contrat additif daté](../data/reference/th18-2026-09-19-baby-dragon-tantrum-contract.json)
classe explicitement ce chemin Core/CLI/GUI/RL comme partiel : ses statistiques
normales, taille de logement, cible air/sol et portée sont accessibles sans
dupliquer de règle dans les adaptateurs. Tantrum vérifie à chaque tick partagé
l'absence d'une autre troupe aérienne alliée dans le rayon sourcé de 4,5 cases,
puis applique x2 dégâts et x1,5 cadence d'attaque (sans modifier sa vitesse de
déplacement). Le test focalisé couvre les cas isolé, allié aérien proche,
allié terrestre proche, snapshot, replay et hash déterministe; la fixture CLI
exerce la même route publique. Comme aucune source consultée ne fixe le moment
exact de bascule pendant une animation ou une attaque déjà engagée, ce contrat
reste une adaptation déterministe, non une fidélité vérifiée. Les tiers visuels
Baby Dragon résolus par le catalogue sont présents dans le pack local et
contrôlés par `validate_viewer_assets.py`; cette présence ne transforme pas la
fidélité de l'animation ou de la silhouette en validation de gameplay.

Le Mineur L1–12 est importé avec son niveau 12 TH18, ses PV, dégâts, cadence,
portée, vitesse et logement. Son état `underground` est une valeur logique
persistée par `COCSIM-SNAPSHOT-11` : pendant le déplacement vers sa cible, il
prend une route directe sous les Murs, n'arme pas les pièges et ne peut pas être
ciblé ou atteint par un projectile défensif ; il refait surface pour attaquer.
Le Viewer affiche un marqueur de tunnel et RL reçoit le même état ainsi que
l'action de déploiement. Les régressions couvrent mur, bombe, surface et
snapshot. La durée exacte d'immersion, les projectiles déjà lancés et les
priorités contre troupes défensives ne sont pas publiés au niveau du tick : le
comportement reste donc explicitement non certifié. Les tiers visuels Mineur
résolus par le catalogue sont présents dans le pack local et vérifiés au
chargement par `validate_viewer_assets.py`; seule la fidélité des états
souterrains visuels demeure ouverte.

Séisme possède ses huit niveaux, rayons et pourcentages importés. Il applique
sur les bâtiments non-stockage `p/(2n−1)` de leurs PV maximaux, applique 25 %
des PV maximum aux murs à chaque impact (quatre impacts les détruisent), et
applique les pourcentages de troupe sol publiés aux niveaux 6–8. La régression
vérifie les deux premiers impacts, les murs et l’exclusion des stockages. Son
compteur par bâtiment est persisté dans `COCSIM-SNAPSHOT-11`, dont le lecteur
reste rétrocompatible V2 à V10. La fidélité reste `a_verifier` en attente de
validation par replay sur les cas à bord de rayon.

Une régression isole le Séisme L6 : une troupe terrestre dans le rayon perd
exactement 5 % de ses PV maximum, tandis qu’un Dragon au même emplacement reste
indemne. Le scénario utilise un Hôtel de Ville TH18 non défensif pour empêcher
une défense annexe de contaminer la mesure.

### Invariant transversal : snapshots et replays

`COCSIM-SNAPSHOT-13` sérialise les types étendus, réserves de sorts, effets
actifs, projectiles, commandes, état de déplacement, verrou mono-cible de
l'Inferno et explosions différées. L'empreinte d'état est
dérivée de cette représentation et du fingerprint du scénario immuable, afin
de refuser une restauration dans une autre disposition. Une régression restaure un effet Hâte
actif et sa réserve consommée. `Replay V2` sérialise aussi les réserves de sorts
et les commandes `cast_spell`; une régression recharge le replay puis compare
l'empreinte du combat rejoué. Le lecteur reste compatible avec `Replay V1` et
les scénarios sans tableau `spells`.

### Performance de chargement

`GameData::v0()` mémorise désormais le catalogue normalisé immuable pendant la
durée du processus. Chaque `BattleState` reçoit toujours sa copie de données ;
seul le re-parsing JSON/regex est évité. La suite `cocsim_tests` repasse après
ce changement, y compris le test de ciblage de Défense aérienne.

## Vérification du dernier incrément

Le Viewer route explicitement tous les pièges aujourd'hui intégrés (`bomb`,
`giant_bomb`, `air_bomb`, `seeking_air_mine`, `spring_trap`) vers
`images/home/traps/...`. Cette vérification fait partie de l'audit TH18 : un
chemin core/RL valide ne suffit donc plus à déclarer un piège visible si son
routage d'asset disparaît. La présence du fichier à la racine d'assets choisie
reste affichée et contrôlable au lancement du Viewer ; elle dépend du pack
d'assets que l'utilisateur fournit.

Gobelin, Serviteur et Chevaucheur de cochon ont leurs niveaux importés et sont
déployables via la même commande déterministe que les autres troupes. Les tests
vérifient leurs entrées, le vol du Serviteur et les priorités Ressources/Défenses.
Les tiers visuels résolus par le catalogue pour ces trois troupes sont présents
dans le pack et `validate_viewer_assets.py` contrôle leur disponibilité. Cela ne
valide pas leurs animations, transparences ni leur fidélité visuelle au client ;
seuls les squelettes invoqués du Piège à squelettes conservent actuellement une
lacune de fichier explicitement déclarée.

## Critère d'avancement par ligne

Chaque élément doit ensuite recevoir une ligne individuelle dans cette matrice
avec la source de données datée, tous ses niveaux/variantes, le test ciblé et le
numéro de régression exécuté. Les regroupements ci-dessus servent uniquement à
figer l'inventaire initial et ne sont pas une déclaration de couverture.

## Audit atomique des éléments déjà chargés

L'audit reproductible `python tools/audit_th18_conformity.py` parcourt chaque
entrée réellement importée, et compare ses niveaux aux deltas officiels du
registre, sa provenance brute épinglée, son `Kind` core, l'énumération RL et les
troupes réellement déployables depuis le Viewer. Il imprime une ligne par
élément et marque `NON_CONFORME` tout trou structurel. Il imprime ensuite un
inventaire inverse : tout élément explicitement documenté dans les deltas
officiels mais absent du catalogue. Ainsi, un contenu TH18 manquant ne peut plus
disparaître d'un audit limité aux entrées déjà chargées. Le Viewer charge le même
JSON via `--scenario` : les troupes de son armée y sont déployables et les
bâtiments défenseurs y sont visibles. `PRESENT_A_VERIFIER` ne vaut jamais
validation des tailles, statistiques, portée ou comportement : ces points
restent à fermer par élément avec une source TH18 et une régression.

Le même audit compare également le catalogue avec des index communautaires
datés : troupes régulières, défenses, bâtiments de ressources/armée, pièges,
héros, familiers et engins. Cette couche est signalée comme `secondaire` et ne
prouve ni niveau, ni statistique, ni comportement ; elle sert strictement à
détecter les omissions que les notes de patch (qui ne sont que des deltas) ne
citent jamais.

Le registre couvre également les deux familles d’attaque qui ne sont pas des
bâtiments : 18 sorts, 17 Super Troupes et 41 équipements de héros. Les comptes
et noms sont figés dans l’index secondaire ; leurs niveaux, leurs effets et
leurs interactions restent à documenter avant toute intégration moteur.

La sortie de l’audit comporte enfin `Matrice de couverture — inventaire
attendu` : une ligne par élément attendu, même absent, avec ses familles et
sources, le statut catalogue/core/GUI/RL, le niveau de test et une fidélité
explicitement conservatrice. `audit_structure` ne signifie pas fidélité de
combat ; seule une régression comportementale sourcée pourra fermer cette
colonne.

Les variantes `supercharged` sont contrôlées séparément. Le catalogue actif
contient 27 enregistrements pour 14 familles : Mine d'or, Collecteur d'élixir,
Tour de l'Enfer, Défense aérienne, X-Bow, Catapulte, Monolithe, Cabane de
constructeur, Canon à ricochet, Tour d'archers multiple, Tour à engrenages,
Tour de Super Sorcier, Tour à bombes et Foreuse d'élixir noir.
Treize familles sont matérialisées par Core : `Placement.variant`, scénario,
replay, snapshot, Viewer et RL les transmettent au même chargeur, sans
confondre ce champ avec les modes d'arme. La régression
`permanent_variant_tests` charge chacune de ces familles, vérifie son état
observable puis sa restauration et son replay. La variante de Cabane de
constructeur reste catalogue-only, comme la Cabane elle-même, car l'acteur
Constructeur et son contrat de réparation ne sont pas documentés. Ces variantes
restent `a_verifier` pour leur comportement particulier et leur provenance
primaire ; elles ne sont toutefois plus artificiellement réduites à leur niveau
normal.

La validation runtime RL ne se limite plus à une liste d’identifiants. Elle lit
chaque ligne réellement construite par `GameData::v0()` via le binding, puis la
compare à chaque niveau/variante pris en charge du catalogue : PV, dégâts, DPS,
soin, portée, vitesse, tailles, cibles, logement, rayons, dégâts de mort,
multiplicateurs et paramètres de pièges. Ainsi, une régression dans le chargeur
C++ est détectée même si le JSON gelé reste intact.

Le test instancie également les **707** combinaisons niveau/variante exposées
par `GameData` via l’adaptateur Python, y compris les bâtiments passifs et les
pièges (dont l’état armé est volontairement caché de l’observation). Il a
permis de normaliser explicitement les armes `weapon` imbriquées de l’Hôtel de
ville : dégâts, DPS, portée, cadence, cible et dégâts/rayon à la mort. Il
protège aussi la règle de mode du X-Bow : le mode normal cible le sol, tandis
que `air_and_ground` est une sélection distincte dans scénario et replay.
Ces preuves de chargement ne valident pas encore les mécaniques Giga ni les
munitions : elles restent ouvertes dans la fidélité comportementale.

Les Town Hall armés sont maintenant reconnus comme défenses à distance par le
core. La régression TH12 vérifie un tir à 70 dégâts avec la cadence et la
portée sourcées ; la régression TH18 vérifie au contraire l'absence de tir
lorsqu'aucune arme active n'est présente dans le registre. Le tir multi-cible
et les effets Giga restent explicitement non conformes tant qu'ils ne disposent
pas de règles complètes et de tests dédiés.

`validate_ranged_classification.py` rapproche la portée catalogue du chemin
projectile explicite du core. Il exclut uniquement la Guérisseuse, dont la
portée est une portée de soin, et échoue si une autre entrée à portée supérieure
à une case n'est pas classée attaquante à distance. Cette garde a notamment
détecté puis protégé le cas du Serviteur.

Le registre contient aussi la passe officielle du 27 avril 2026 (« Sound of
Clash »). Elle rend immédiatement visibles les niveaux manquants dans le
catalogue historique : Barbarian 13, Goblin 10, Valkyrie 12, Golem 15, Dragon
13, Balloon 13, X-Bow 13, Giant Bomb 12, Army Camp 14, Workshop 9 et Blacksmith
10. Les notes ne publient pas leurs tableaux numériques : aucun niveau n'a donc
été synthétisé. L'audit les marque `NON_CONFORME` jusqu'à l'obtention d'une
source chiffrée, l'import et les régressions correspondantes. Les entrées
initialement absentes Logger et Sky Wagon disposent désormais chacune d’un
artefact local haché et restent distinctement `données seulement` : leur
présence au catalogue ne valide ni le Gardien ni le cycle d’engin de siège.

Les valeurs actuellement publiées par Goblins Farm pour ces onze niveaux sont
gelées, URL par URL et champ par champ, dans
[`sound-of-clash-level-observations-2026-09-17.json`](../data/reference/sound-of-clash-level-observations-2026-09-17.json).
L'artefact est haché par le registre et contrôlé par le validateur. Les trois
bâtiments passifs Army Camp L14, Workshop L9 et Blacksmith L10 sont importés :
leurs PV, coût, durée, TH et niveau ne portent aucune règle de combat manquante.
Les autres observations ne deviennent pas automatiquement une donnée de combat :
les champs explicitement manquants, comme le Golemite du Golem L15 ou les
munitions de X-Bow, interdisent encore la conclusion « conforme ».

Les niveaux `dark_barracks` 13 et `dark_spell_factory` 8, absents du snapshot
d'avril, sont désormais injectés par l'importeur comme extensions déclaratives
depuis un artefact secondaire local haché : respectivement 1 100 et 1 070 PV,
empreinte 3×3. Ils restent marqués secondaires. Les types de pièges déjà
chargés mais sans `Kind` core sont signalés comme non conformes, au lieu d'être
comptés comme intégrés. La Bombe aérienne et la Mine chercheuse sont désormais
des pièges armés : invisibles avant déclenchement, la première frappe les unités
aériennes dans le rayon de zone, la seconde une seule cible aérienne. Le Ressort
sélectionne la plus grande troupe terrestre dans son rayon ; il l’éjecte si son
logement ne dépasse pas sa capacité, sinon applique les dégâts source. Les
régressions couvrent leurs dégâts, sélection et deux branches de capacité.
La source officielle du 24 mars 2025 confirme que le squelette aérien invoqué
par le Piège à squelettes occupe 1 logement (réduction depuis 2). Une source
secondaire datée documente la sélection exclusive sol ou air du piège et son
déclenchement uniquement pour l'altitude choisie. Les règles de durée, géométrie
de spawn, reciblage et distraction ne sont toujours pas publiées sous une forme
déterministe : elles restent ouvertes. Tornade et Giga bombe restent également
partielles.

Les deux sous-squelettes sont chargés et observables par le même core dans le
Viewer et RL, mais leur source de statistiques ne fournit aucun chemin d'image.
La page secondaire `clashwiki-2026-09-17-skeleton-trap-behavior` confirme le
mode aérien à deux ballons rouges et le déclenchement exclusif par altitude,
mais ne fournit pas de sprite individuel réutilisable : cet écart visuel reste
donc ouvert, sans asset substitué.
Le Viewer les représente donc par son rendu générique et le validateur imprime
`OPEN_VISUAL_GAP`; ils restent non conformes visuellement, sans asset inventé.

### Projectiles — incrément déterministe

Les tirs à distance sont des objets Core séparés, sérialisés et hashés ; ils ne
sont pas des dégâts hitscan. Les armes sans vitesse de vol publique restent au
contrat historique d'un impact au tick suivant, explicitement non fidèle au
client. La Mine aérienne chercheuse est l'exception actuellement sourcée : la
note Supercell du 11 août 2021 fixe son projectile à 3,5 cases/s. Son projectile
homing progresse exclusivement par `kTickMs`, est exposé en lecture seule au GUI
et à l'interface RL, et survit à un snapshot/replay. Le GUI dessine sa ligne de
vol et son projectile sans modifier la simulation. Si cette cible est détruite
lors du vol, le Core conclut une unique occurrence terminale sans dégât ; ce
choix déterministe est snapshoté, rejoué et régressé, mais reste une politique
ouverte tant qu'une source de client ne publie pas son ordre exact. Le délai de
lancement, le rayon de collision, l'annulation et la confirmation version TH18
restent dans le registre de delta et empêchent toute revendication de fidélité
complète.
Le Piège à squelettes L1–5 déclenche maintenant selon son mode explicite
`ground` ou `air`, invoque le nombre et le niveau publiés de sous-squelettes,
et conserve cet état dans snapshot/replay via la même entité core. Les tests
couvrent les deux altitudes, le niveau 5 et une restauration de snapshot. La
géométrie exacte des invocations, la durée et l'IA restent non conformes tant
qu'une source déterministe ne les publie pas.

Le Guérisseur dispose maintenant d’une régression comportementale distincte :
après qu’un Canon blesse un Barbare allié, il acquiert cette cible et émet un
projectile logique de soin ; le soin positif n'arrive qu'au tick d'impact, y
compris après restauration. Cette preuve couvre l’acquisition alliée et le soin
effectif, pas encore les interactions particulières avec héros ou bâtiments.

Lorsque le binding Python est configuré, `th18_rl_runtime_validation` crée ce
même scénario via `CoCSimEnv`, passe par son index d'action puis vérifie les
cinq invocations observées. Il exécute aussi l’index d’action réel de chacune
des quinze troupes actuellement déployables et vérifie leur observation. C’est
un contrôle runtime supplémentaire à la validation statique de l’énumération
RL.

La régression Ressort couvre aussi un snapshot/restauration avant déclenchement,
afin de protéger l’état armé caché dans le replay et l’empreinte déterministe.

Le catalogue préserve maintenant aussi les champs source `spawned_units` et
`spawned_unit_level` du Piège à squelettes, `effect_duration_seconds` de la
Tornade et le seuil de logement/dégâts de la Giga bombe. Ces valeurs sont
vérifiées par le validateur TH18. Les mécanismes sourcés de déclenchement,
dégâts et apparition sont exécutés ; seules les règles de durée de vie, de
reciblage/IA, de traction et de recul restent non implémentées faute de données
suffisamment sourcées.

Les statistiques des Squelettes invoqués (PV, dégâts, cadence, vitesse et
portée des variantes sol/air niveaux 1–2) sont désormais consignées comme
source secondaire. La table secondaire épinglée établit pour les deux variantes
une cadence de 0,7 s et des dégâts par coup de 17,5 / 21 aux niveaux 1 / 2 ;
la différence documentée porte sur l'altitude cible et la vitesse. Le moteur
invoque bien le nombre et le niveau sourcés de
squelettes sol ou air, avec une origine logique déterministe commune au core,
Viewer et RL. Leur durée de vie, leur IA/rétention/ciblage, leur géométrie
d'apparition exacte et leur asset source ne le sont pas : le Piège à squelettes
reste donc non conforme, et non simplement « terminé ».

L'audit vérifie aussi la complétude minimale des données de combat par famille et
par niveau : PV/dégâts/cadence/cible/mouvement/logement pour les troupes (soin
pour la Guérisseuse), PV/dégâts/portée/empreinte/cible pour les défenses,
PV/empreinte pour les bâtiments passifs, et les paramètres propres à chaque
piège. `complet` signifie uniquement que ces champs importés existent ; la
provenance primaire et la fidélité d'exécution demeurent des critères séparés.

La régression `CATALOGUE_MATERIALIZATION_EXHAUSTIVE_ELIGIBLE` instancie ensuite
chaque niveau normal ou Supercharge que le Core matérialise directement, puis
compare les PV maximum, portée, emprise, catégorie, cible, vol et soin de la
vue de bataille aux `Stats` immuables du catalogue. Elle évite délibérément de
simuler le pathfinding pendant ce contrôle de câblage. Les pièges armés, la
Tesla cachée et les unités invoquées ont une construction conditionnelle et
conservent leurs régressions de machine d'état dédiées. Cette preuve renforce
la colonne Tests de la matrice ; elle ne valide ni une statistique contre le
client, ni les mécaniques particulières.

Pour les sorts actuellement exposés, `SPELL_MATERIALIZATION_EXHAUSTIVE` lance
chaque niveau via une commande future `T+10 ms`, comme le GUI et RL. Rage, Soin,
Gel, Hâte et Saut doivent conserver leur rayon et leur échéance issus du
catalogue ; Foudre et Séisme doivent produire respectivement leurs dégâts et
pourcentage instantanés importés. Cette régression contrôle le câblage de tous
les niveaux, mais ne transforme pas les interactions de bord, le ciblage ou
l’animation du client en fidélité vérifiée.

Le multiplicateur Ressources du Goblin est une donnée de catalogue traçable :
`resource_damage_multiplier = 2`, applicable aux Mines, Collecteurs, Foreuses,
Stockages et à l'Hôtel de Ville. Le moteur l'évalue contre la catégorie de la
cible (et non via une constante d'entité), et une régression mesure 22 dégâts
pour le Goblin L1 (11 dégâts de base) sur une Mine d'or. La source reste
secondaire ; le comportement de repli après épuisement des cibles Ressources
doit encore être confronté à des replays de référence.

Le Golemite est un contenu `spawned_only` distinct et haché : il ne peut pas
être ajouté à l'armée ni déployé, mais naît à la destruction de son Golem par
la même boucle core observée par le Viewer et RL. Les quinze niveaux conservent
PV, dégâts, DPS, dégâts/rayon à la mort, logement, vitesse, cadence, portée et
progression d'invocation 2/3/4. La régression couvre le Golem L15 détruit et
ses quatre invocations. Les sources publiques ne donnent pas les positions
exactes d'apparition : le moteur les place donc exactement au point de mort,
de façon déterministe. Cette approximation reste ouverte et le Golem ne passe
pas en fidélité vérifiée pour cette raison.

Le Viewer ne reconstruit plus un nom d'image `level-N.png`. Il consulte le
champ `image` du niveau catalogue, donc réutilise exactement les tiers visuels
source quand plusieurs niveaux partagent une apparence. Pour un niveau ajouté
après le snapshot d'assets, il choisit explicitement le dernier tier visuel
sourcé ; ce fallback est compté par `validate_viewer_assets.py`, qui vérifie
aussi que tous les fichiers d'image déclarés sont réellement présents dans le
pack local. Cela supprime les faux sprites manquants sans prétendre qu'un
fallback est le nouvel artwork TH18 absent des sources épinglées.

La colonne `Tests` de la matrice générée distingue désormais
`rl_runtime:catalogue` (comparaison de la ligne GameData réelle avec le JSON
gelé), `core_behavior` (au moins une assertion de comportement exercée par la
suite C++) et `audit_structure` (chemin de données seulement). Par exemple, la
régression Guérisseur déploie un Barbare blessé sous le feu d'un Canon et
vérifie l'événement de soin. Ces preuves ne transforment pas encore une ligne
en fidélité vérifiée : les paramètres et les interactions restantes doivent
toujours être confrontés à leur provenance TH18.
