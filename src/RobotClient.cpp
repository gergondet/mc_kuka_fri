#include "RobotClient.h"

namespace mc_kuka_fri
{

RobotClient::RobotClient(AppState & state, const std::string & name)
: state_(state), app_(connection_, *this), name_(name), torques_measured_(7, 0.0), joints_measured_(7, 0.0),
  torques_command_(7, 0.0), joints_command_(7, 0.0)
{
  auto config = state.gc.configuration().config("KukaFRI")(name);
  std::string host = config("host");
  int port = config("port", 30200);
  if(!app_.connect(port, host.c_str()))
  {
    mc_rtc::log::error_and_throw("[MCKukaFRI] Connection to {}:{} failed", host, port);
  }
  // XXX Do we need to do this or can we be sure that we have a valid state after connection?
  kuka::fri::LBRClient::waitForCommand();
  // Check that the robot has a compatible command mode
  auto mode = robotState().getClientCommandMode();
  if(mode != kuka::fri::TORQUE && mode != kuka::fri::POSITION)
  {
    mc_rtc::log::error_and_throw("[MCKukaFRI] Only support TORQUE and POSITION control modes");
  }
  updateMcRtcInputs();
}

void RobotClient::waitForCommand()
{
  kuka::fri::LBRClient::waitForCommand();

  std::lock_guard<std::mutex> lck(state_.gc_mutex);
  updateMcRtcInputs();
}

void RobotClient::updateMcRtcInputs()
{
  const auto & state = robotState();
  std::memcpy(joints_measured_.data(), state.getMeasuredJointPosition(), 7 * sizeof(double));
  std::memcpy(torques_measured_.data(), state.getMeasuredTorque(), 7 * sizeof(double));
  state_.gc.setEncoderValues(name_, joints_measured_);
  state_.gc.setJointTorques(name_, torques_measured_);
}

void RobotClient::command()
{
  kuka::fri::LBRClient::command();

  std::lock_guard<std::mutex> lck(state_.gc_mutex);
  updateKukaCommand();
}

void RobotClient::updateKukaCommand()
{
  const auto & robot = state_.gc.robot(name_);
  for(size_t i = 0; i < 7; ++i)
  {
    auto mbcIdx = robot.jointIndexInMBC(i);
    torques_command_[i] = robot.jointTorque()[mbcIdx][0];
    joints_command_[i] = robot.q()[mbcIdx][0];
  }
  robotCommand().setJointPosition(joints_command_.data());
  if(robotState().getClientCommandMode() == kuka::fri::TORQUE) { robotCommand().setTorque(torques_command_.data()); }
}

void RobotClient::startControlThread()
{
  control_thread_ = std::thread(
      [this]()
      {
        bool ok = true;
        while(ok) { ok = app_.step(); }
      });
}

void RobotClient::joinControlThread()
{
  if(!control_thread_.joinable()) { return; }
  control_thread_.join();
  app_.disconnect();
}

void MainRobotClient::command()
{
  kuka::fri::LBRClient::command();

  std::lock_guard<std::mutex> lck(state_.gc_mutex);
  state_.gc.run();
  updateKukaCommand();
}

} // namespace mc_kuka_fri
