#include "cocsim/core.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <stdexcept>
namespace py=pybind11;
using namespace cocsim;
class NativeBattle {
 public:
  explicit NativeBattle(std::uint64_t seed=1) { Scenario s; s.seed=seed; state_=BattleState(s); }
  void reset(std::uint64_t seed=1) { Scenario s; s.seed=seed; state_=BattleState(s); }
  void load_scenario_json(const std::string& text) {
    Scenario s; std::string error;
    if (!parse_scenario(text,s,error)) throw std::invalid_argument(error);
    state_=BattleState(s);
  }
  void wait_at(Milliseconds effective) { submit(CommandType::Wait,effective); }
  void end_at(Milliseconds effective) { submit(CommandType::EndBattle,effective); }
  void advance_ticks(std::uint64_t ticks) { state_.advance_ticks(ticks); }
  std::string snapshot() const { return state_.snapshot(); }
  void restore(const std::string& data) { std::string error; if (!state_.restore(data,error)) throw std::invalid_argument(error); }
  std::string replay_json() const { return cocsim::replay_json(state_.scenario(),state_.commands()); }
  py::dict observation() const {
    auto o=state_.observe(); py::dict d;
    d["time_ms"]=o.time_ms; d["width"]=o.width; d["height"]=o.height;
    d["result"]=result_name(o.result); d["state_hash"]=o.state_hash;
    d["entities"]=py::list(); return d;
  }
 private:
  BattleState state_;
  void submit(CommandType type,Milliseconds at) { std::string error; if (!state_.submit(type,at,error)) throw std::invalid_argument(error); }
};
PYBIND11_MODULE(_cocsim,m) {
  m.attr("tick_ms")=kTickMs;
  py::class_<NativeBattle>(m,"NativeBattle")
    .def(py::init<std::uint64_t>(),py::arg("seed")=1)
    .def("reset",&NativeBattle::reset,py::arg("seed")=1)
    .def("load_scenario_json",&NativeBattle::load_scenario_json)
    .def("wait_at",&NativeBattle::wait_at)
    .def("end_at",&NativeBattle::end_at)
    .def("advance_ticks",&NativeBattle::advance_ticks)
    .def("snapshot",&NativeBattle::snapshot)
    .def("restore",&NativeBattle::restore)
    .def("replay_json",&NativeBattle::replay_json)
    .def("observation",&NativeBattle::observation);
}
