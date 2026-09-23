# Protocole de mesure applicative TH18

Ce protocole ne donne aucune autorisation d'accéder, extraire, modifier ou
automatiser l'application. Il prépare seulement des relevés reproductibles
lorsqu'un utilisateur autorisé exécute lui-même des scénarios dans le client.

## Règles de capture

- Consigner version du client, plateforme, date/heure, mode de bataille et
  niveau/variante exacts avant chaque essai.
- Employer une base vide et un seul élément à mesurer ; répéter avec les mêmes
  placements pour identifier les départages par distance et par identifiant.
- Conserver le replay et une vidéo à fréquence connue. Une cadence d'au moins
  120 images/s est nécessaire pour proposer une mesure à 10 ms ; une vidéo à
  30 ou 60 images/s ne prouve pas un délai de tick.
- Relever les positions en tuiles à partir de bâtiments d'emprise connue, sans
  déduire une géométrie interne depuis les pixels seuls.
- Noter toute ambiguïté, image manquante ou résultat contradictoire. Une
  observation n'est importée que si ses unités, son niveau et sa version sont
  identifiés ; sinon elle reste une piste dans un delta.

## Priorités de mesure

| Groupe | Relevé minimal déterministe |
| --- | --- |
| Furnace / Firemite | Instant de déploiement, premier spawn, chaque spawn, mort/auto-dégât, position enfant, cible, saut, impact, centre/rayon/durée/pulses du feu et chevauchement de deux feux. |
| Ruin Witch / Ruin Knight | Position et durée de rubble, début/fin de travail, début/fin d'invocation, position/orientation du Knight, maximum vivant, comportement si Witch détruite pendant l'invocation. |
| Meteor Golem / Meteormite | Seuil de division, positions/retards de lancer et atterrissage, nombre et niveaux enfants, invulnérabilité, cible, fusion et son déclencheur. |
| Air Sweeper | Origine/axe du cône, bords angulaires, cibles égales, déplacement par pulsation, collision avec murs/bâtiments et effet en bordure. |
| Spell Tower | Condition de déclenchement, délai, centre/portée, durée/pulses, destruction/annulation, recharge et ordre avec plusieurs cibles. |
| Builder's Hut / Builder | Moment de spawn, cible choisie et réaffectation, trajet/collision, quantité réparée à chaque intervalle, simultanéité de plusieurs Builders et disparition à la destruction de la Cabane. |
| Assets de sous-unités | Capture d'un écran d'information et d'un écran de bataille par niveau/variante. Noter le nom affiché et la version ; une capture ne devient pas un fichier Viewer sans autorisation explicite de son usage. |

## Format de restitution

Chaque essai doit fournir les métadonnées, le scénario et les mesures brutes :

```json
{
  "client_version": "…",
  "platform": "…",
  "captured_at": "YYYY-MM-DDTHH:MM:SS+02:00",
  "mode": "friendly_challenge",
  "subject": {"kind": "…", "level": 1, "variant": "normal"},
  "scenario": {"placements": [], "commands": []},
  "video": {"frame_rate_hz": 120, "replay_identifier": "…"},
  "observations": [],
  "uncertainties": []
}
```

Le moteur n'utilise jamais directement ce fichier : après revue, les valeurs
reproductibles sont ajoutées à un delta daté, hashé et rattaché à la chaîne de
références. Les règles encore ambiguës restent désactivées.
