#include "cocsim/core.hpp"
#include "detail/battle_state_internal.hpp"
#include "detail/geometry.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <numbers>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace cocsim {

using detail::dist;

EntityId BattleState::choose_target(const Entity& a) const {
  const Entity* best=nullptr;
  const auto category_of=[&](const Entity& candidate){const auto* stats=data_.find(candidate.kind,candidate.level);return stats?stats->category:EntityCategory::Other;};
  const auto matches_focus=[&](const Entity& candidate){const auto category=category_of(candidate);if(a.target_focus==TargetFocus::Defenses||a.target_focus==TargetFocus::DefensesOnly)return category==EntityCategory::Defense;if(a.target_focus==TargetFocus::Resources)return category==EntityCategory::Resource;if(a.target_focus==TargetFocus::Walls)return category==EntityCategory::Wall;return category!=EntityCategory::Wall;};
  const auto can_target_altitude=[&](const Entity& candidate){return a.target_type==TargetType::Both||(a.target_type==TargetType::Air?candidate.flying:!candidate.flying);};
  if(a.heals) {
    for(const auto& e:entities_) if(alive(e)&&e.side==a.side&&e.id!=a.id&&category_of(e)==EntityCategory::Troop&&e.hp<e.max_hp) {
      if(!best||dist(a.pos,e.pos)<dist(a.pos,best->pos)||(dist(a.pos,e.pos)==dist(a.pos,best->pos)&&e.id<best->id)) best=&e;
    }
    return best?best->id:0;
  }
  for(int pass=0;pass<2&&!best;++pass) for(const auto& e:entities_) if(alive(e)&&e.side!=a.side&&!(a.side==Side::Attacker&&e.concealed)&&!e.underground&&!(a.side==Side::Defender&&e.invisible_to_defenses_until>now_ms_)&&category_of(e)!=EntityCategory::Trap&&can_target_altitude(e)&&dist(a.pos,e.pos)>=a.min_range
      // Mobile defender-side spawns use the ordinary troop route. An immobile
      // defense must never reserve a distant target: doing so would prevent it
      // from acquiring a later troop that has actually entered its range.
      && !(a.side==Side::Defender&&a.speed<=0.0&&dist(a.pos,e.pos)>a.range+a.radius+e.radius)) {
    if(pass==0&&!matches_focus(e)) continue;
    if(!best||dist(a.pos,e.pos)<dist(a.pos,best->pos)||(dist(a.pos,e.pos)==dist(a.pos,best->pos)&&e.id<best->id)) best=&e;
  }
  return best?best->id:0;
}
} // namespace cocsim
