#include "RobotClient.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

int main(int argc, char * argv[])
{
  std::string conf_file = "";
  po::options_description desc("MCFrankaControl options");
  // clang-format off
  desc.add_options()
    ("help", "Display help message")
    ("conf,f", po::value<std::string>(&conf_file), "Configuration file");
  // clang-format on

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if(vm.count("help"))
  {
    std::cout << desc << "\n";
    return 0;
  }

  mc_control::MCGlobalController::GlobalConfiguration gconfig(conf_file, nullptr);
  if(!gconfig.config.has("KukaFRI"))
  {
    mc_rtc::log::error_and_throw<std::runtime_error>(
        "No KukaFRI section in the configuration, see README for an example");
  }
  mc_kuka_fri::AppState state(gconfig);

  auto kukaConfig = gconfig.config("KukaFRI");
  auto ignored = kukaConfig("ignored", std::vector<std::string>{});

  std::vector<mc_kuka_fri::RobotClientPtr> clients;
  for(const auto & r : state.gc.robots())
  {
    if(r.mb().nrDof() == 0 || std::find(ignored.begin(), ignored.end(), r.name()) != ignored.end()) { continue; }
    if(!kukaConfig.has(r.name()))
    {
      mc_rtc::log::error_and_throw("[MCKukaFRI] {} is an active robot but is not configured in KukaFRI, either add a "
                                   "configuration for this robot or add it to the ignored list",
                                   r.name());
    }
    if(clients.empty()) { clients.push_back(std::make_unique<mc_kuka_fri::MainRobotClient>(state, r.name())); }
    else { clients.push_back(std::make_unique<mc_kuka_fri::RobotClient>(state, r.name())); }
  }
  state.gc.init();
  state.gc.running = true;
  for(auto & client : clients) { client->startControlThread(); }
  for(auto & client : clients) { client->joinControlThread(); }
  return 0;
}
