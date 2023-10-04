#pragma once

#include <mc_control/mc_global_controller.h>

namespace mc_kuka_fri
{

/** Data shared by all robots and the application */
struct AppState
{
  std::mutex gc_mutex;
  mc_control::MCGlobalController gc;

  inline AppState(const mc_control::MCGlobalController::GlobalConfiguration & config) : gc(config) {}
};

} // namespace mc_kuka_fri
