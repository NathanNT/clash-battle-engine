#include "cocsim/core.hpp"
#include <iostream>
#include <limits>
#include <string>

using namespace cocsim;
namespace {
Milliseconds round_up_tick(Milliseconds value) {
  if (value < 0) throw std::invalid_argument("time must be non-negative");
  const auto remainder = value % kTickMs;
  if (remainder == 0) return value;
  const auto increment = kTickMs - remainder;
  if (value > std::numeric_limits<Milliseconds>::max() - increment)
    throw std::out_of_range("time exceeds representable tick range");
  return value + increment;
}
Scenario demo() { Scenario s; s.width=30;s.height=20;s.duration_ms=60000; s.defenders={{Kind::Cannon,1,{15,10}},{Kind::Wall,1,{11,10}},{Kind::TownHall,1,{20,10}}}; s.army={{Kind::Barbarian,1,10},{Kind::Archer,1,4},{Kind::Giant,1,1}}; return s; }
void usage(){std::cerr<<"Usage: cocsim demo [--until MS] [--trace] [--projectiles]\n       cocsim validate <scenario.json>\n       cocsim simulate <scenario.json> [--deploy KIND X Y [AT_MS]] [--until MS] [--trace] [--projectiles] [--save-replay PATH]\n       cocsim replay <replay.json> [--until MS] [--trace] [--projectiles]\n       cocsim hero-support-catalogue\n       cocsim hero-support-level <id> <level>\n";}
int print_hero_support_catalogue(){try{const auto data=GameData::v0();const auto& rows=data.hero_support_catalogue();std::cout<<"{\"combat_status\":\"catalogued_only\",\"items\":[";for(std::size_t i=0;i<rows.size();++i){const auto& row=rows[i];if(i)std::cout<<',';std::cout<<"{\"id\":\""<<row.id<<"\",\"category\":\""<<row.category<<"\",\"hero\":\""<<row.hero<<"\",\"max_level\":"<<row.max_level<<",\"support\":\""<<row.support<<"\",\"ability_type\":\""<<row.ability_type<<"\"}";}std::cout<<"]}\n";return 0;}catch(const std::exception& e){std::cerr<<"catalogue error: "<<e.what()<<"\n";return 1;}}
int print_hero_support_level(const std::string& id,const std::string& level_text){
  try {
    std::size_t end=0;
    const int level=std::stoi(level_text,&end);
    if(end!=level_text.size()||level<1){std::cerr<<"invalid Hero support level\n";return 2;}
    const auto data=GameData::v0();
    const auto* entry=data.find_hero_support(id);
    const auto* row=data.find_hero_support_level(id,level);
    if(!entry||!row){std::cerr<<"unknown Hero support ID or level\n";return 2;}
    std::cout<<"{\"combat_status\":\"catalogued_only\",\"content\":"<<entry->normalized_content_json
             <<",\"level_record\":"<<row->normalized_row_json<<"}\n";
    return 0;
  } catch(const std::invalid_argument&) { std::cerr<<"invalid Hero support level\n"; return 2; }
    catch(const std::out_of_range&) { std::cerr<<"invalid Hero support level\n"; return 2; }
    catch(const std::exception& e) { std::cerr<<"catalogue error: "<<e.what()<<"\n"; return 1; }
}
int run(Scenario scenario,std::vector<Command> commands,Milliseconds until,bool trace,bool show_projectiles,const std::string& replay){
  try {
    BattleState b(GameData::v0(),scenario);
    for(auto&c:commands){std::string error;if(!b.submit(c,&error)){std::cerr<<"command rejected: "<<error<<"\n";return 2;}}
    if(!replay.empty()){std::string error;if(!save_replay(replay,scenario,commands,error)){std::cerr<<"replay error: "<<error<<"\n";return 2;}}
    // A scenario or inspection deadline can be any integer millisecond.
    // Execute through its first representable fixed-tick boundary.
    b.advance_to(round_up_tick(until));
    if(trace)for(const auto&e:b.events())std::cout<<"T+"<<e.time_ms<<"ms "<<static_cast<int>(e.type)<<" "<<e.detail<<"\n";
    if(show_projectiles)for(const auto& p:b.observe_projectiles())std::cout<<"projectile id="<<p.id<<" owner="<<p.owner<<" target="<<p.target<<" origin="<<p.origin.x<<','<<p.origin.y<<" position="<<p.position.x<<','<<p.position.y<<" speed_tiles_per_second="<<p.speed_tiles_per_second<<" launched_ms="<<p.launched_ms<<" homing="<<p.homing<<"\n";
    std::cout<<to_json(b.result())<<"\nhero_loadouts="<<to_json(b.scenario().hero_loadouts)
             <<"\nmonolith_arrow_housing_projection="<<b.monolith_arrow_deployed_housing()
             <<" tier_projection="<<b.monolith_arrow_housing_tier()
             <<" damage_percent_projection="<<b.monolith_arrow_damage_percent()
             <<"\nstate_hash="<<b.state_hash()<<"\n";
    return b.result().finished?0:3;
  } catch(const std::exception&e){std::cerr<<"error: "<<e.what()<<"\n";return 1;}
}
}
int main(int argc,char**argv){if(argc<2){usage();return 2;}std::string mode=argv[1];if(mode=="hero-support-catalogue"){if(argc!=2){usage();return 2;}return print_hero_support_catalogue();}if(mode=="hero-support-level"){if(argc!=4){usage();return 2;}return print_hero_support_level(argv[2],argv[3]);}if(mode=="validate"){if(argc!=3){usage();return 2;}Scenario s;std::string error;if(!load_scenario(argv[2],s,error)){std::cerr<<"invalid scenario: "<<error<<"\n";return 2;}try{BattleState checked(GameData::v0(),s);}catch(const std::exception& e){std::cerr<<"invalid scenario: "<<e.what()<<"\n";return 2;}std::cout<<"valid scenario: "<<s.defenders.size()<<" defenders, "<<s.army.size()<<" army slots\n";return 0;}Scenario s;std::vector<Command> commands;int first_option=2;if(mode=="demo"){s=demo();commands={{CommandType::Deploy,kTickMs,0,0,Kind::Giant,1,{1,10}},{CommandType::Deploy,kTickMs,0,0,Kind::Barbarian,1,{1,9}},{CommandType::Deploy,kTickMs,0,0,Kind::Barbarian,1,{1,11}},{CommandType::Deploy,round_up_tick(1000),0,0,Kind::Archer,1,{1,8}}};}else if(mode=="simulate"){if(argc<3){usage();return 2;}std::string error;if(!load_scenario(argv[2],s,error)){std::cerr<<"invalid scenario: "<<error<<"\n";return 2;}first_option=3;}else if(mode=="replay"){if(argc<3){usage();return 2;}std::string error;if(!load_replay(argv[2],s,commands,error)){std::cerr<<"invalid replay: "<<error<<"\n";return 2;}first_option=3;}else{usage();return 2;}Milliseconds until=s.duration_ms;bool trace=false,show_projectiles=false;std::string replay;for(int i=first_option;i<argc;++i){std::string a=argv[i];if(a=="--trace")trace=true;else if(a=="--projectiles")show_projectiles=true;else if(a=="--until"&&i+1<argc)until=std::stoll(argv[++i]);else if(a=="--save-replay"&&i+1<argc)replay=argv[++i];else if(a=="--deploy"&&i+3<argc){auto kind=parse_kind(argv[++i]);if(!kind){std::cerr<<"unknown troop\n";return 2;}double x=std::stod(argv[++i]),y=std::stod(argv[++i]);Milliseconds at=kTickMs;if(i+1<argc&&std::string(argv[i+1]).rfind("--",0)!=0)at=std::stoll(argv[++i]);const auto slot=std::find_if(s.army.begin(),s.army.end(),[&](const ArmySlot& candidate){return candidate.kind==*kind&&candidate.count>0;});const int level=slot==s.army.end()?1:slot->level;commands.push_back({CommandType::Deploy,at,0,0,*kind,level,{x,y}});}else{usage();return 2;}}if(until<0){std::cerr<<"--until must be non-negative\n";return 2;}return run(std::move(s),std::move(commands),until,trace,show_projectiles,replay);}
