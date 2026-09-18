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

void BattleState::cast_spell(const Command& c) {
  if(c.position.x<0||c.position.y<0||c.position.x>=scenario_.width||c.position.y>=scenario_.height) { emit(EventType::Rejected,0,0,0,"invalid spell position"); return; }
  const auto* stats=data_.find_spell(c.spell,c.level);
  auto slot=std::find_if(scenario_.spells.begin(),scenario_.spells.end(),[&](auto& s){return s.kind==c.spell&&s.level==c.level&&s.count>0;});
  if(!stats||slot==scenario_.spells.end()) { emit(EventType::Rejected,0,0,0,"spell reserve exhausted or unsupported level"); return; }
  --slot->count;
  emit(EventType::Accepted,0,0,0,"cast "+to_string(c.spell));
  if(c.spell==SpellKind::Lightning) {
    for(auto& target:entities_) if(alive(target)&&target.side==Side::Defender&&dist(target.pos,c.position)<=stats->radius) damage(0,target.id,stats->damage);
    emit(EventType::Attack,0,0,stats->damage,"lightning");
    return;
  }
  if(c.spell==SpellKind::Earthquake) {
    const auto is_storage=[](Kind k){return k==Kind::GoldStorage||k==Kind::ElixirStorage||k==Kind::DarkElixirStorage;};
    for(auto& target:entities_) if(alive(target)&&dist(target.pos,c.position)<=stats->radius) {
      if(target.side==Side::Defender&&target.kind==Kind::Wall) { damage(0,target.id,target.max_hp*.25); ++target.earthquake_hits; }
      else if(target.side==Side::Defender&&!is_storage(target.kind)) { const auto percent=stats->building_damage_percent/100.0; damage(0,target.id,target.max_hp*percent/(2*target.earthquake_hits+1)); ++target.earthquake_hits; }
      else if(target.side==Side::Attacker&&!target.flying&&stats->troop_damage_percent>0) damage(0,target.id,target.max_hp*stats->troop_damage_percent/100.0);
    }
    emit(EventType::Attack,0,0,stats->building_damage_percent,"earthquake");
    return;
  }
  SpellEffect effect; effect.kind=c.spell; effect.pos=c.position; effect.radius=stats->radius; effect.expires=now_ms_+stats->duration;
  effect.damage_multiplier=stats->damage_multiplier; effect.speed_multiplier=stats->speed_multiplier;
  if(c.spell==SpellKind::Heal) { effect.pulses=stats->pulses; effect.pulse_interval=std::max<Milliseconds>(kTickMs,stats->pulse_interval); effect.next_pulse=now_ms_; effect.healing_per_pulse=stats->total_healing/std::max(1,stats->pulses); }
  spell_effects_.push_back(effect);
}
void BattleState::update_spells() {
  for(auto& effect:spell_effects_) if(effect.kind==SpellKind::Heal) while(effect.pulses>0&&effect.next_pulse<=now_ms_) {
    for(auto& target:entities_) if(alive(target)&&target.side==Side::Attacker&&dist(target.pos,effect.pos)<=effect.radius) { const auto old=target.hp; target.hp=std::min(target.max_hp,target.hp+effect.healing_per_pulse); emit(EventType::Healed,0,target.id,target.hp-old,"heal spell"); }
    --effect.pulses; effect.next_pulse+=effect.pulse_interval;
  }
  spell_effects_.erase(std::remove_if(spell_effects_.begin(),spell_effects_.end(),[&](const auto& effect){return effect.expires<=now_ms_||(effect.kind==SpellKind::Heal&&effect.pulses<=0);}),spell_effects_.end());
}
} // namespace cocsim

