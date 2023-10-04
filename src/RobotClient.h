#pragma once

#include "AppState.h"

#include <kuka/fri/ClientApplication.h>
#include <kuka/fri/LBRClient.h>
#include <kuka/fri/UdpConnection.h>

namespace mc_kuka_fri
{

/** Bridge a robot from mc_rtc controller to KUKA FRI */
struct RobotClient : public kuka::fri::LBRClient
{
  /** Creates the client and connect to the robot */
  RobotClient(AppState & state, const std::string & name);

  void waitForCommand() override;

  void command() override;

  void startControlThread();

  void joinControlThread();

protected:
  AppState & state_;
  kuka::fri::UdpConnection connection_;
  kuka::fri::ClientApplication app_;
  std::string name_;
  std::vector<double> torques_measured_;
  std::vector<double> joints_measured_;
  std::vector<double> torques_command_;
  std::vector<double> joints_command_;
  std::thread control_thread_;

  void updateMcRtcInputs();

  void updateKukaCommand();
};

/** Similar to \ref RobotClient but runs mc_rtc's main loop */
struct MainRobotClient : public RobotClient
{
  using RobotClient::RobotClient;

  void command() override;
};

using RobotClientPtr = std::unique_ptr<RobotClient>;

} // namespace mc_kuka_fri
