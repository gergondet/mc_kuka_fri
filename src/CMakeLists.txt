set(app_SRC main.cpp RobotClient.cpp RobotClient.h AppState.h)
add_executable(MCKukaFRI ${app_SRC})
target_link_libraries(MCKukaFRI PUBLIC mc_rtc::mc_control KUKA_FRI::KUKA_FRI
                                       Boost::program_options)
install(TARGETS MCKukaFRI DESTINATION ${CMAKE_INSTALL_FULL_BINDIR})
