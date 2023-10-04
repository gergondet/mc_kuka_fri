cmake_minimum_required(VERSION 3.10)

project(mc_kuka_fri LANGUAGES CXX VERSION 1.0.0)

find_package(mc_rtc REQUIRED)

set(CATKIN_INSTALL_INTO_PREFIX_ROOT FALSE)
find_description_package(iiwa_description)

option(WITH_KUKA_FRI "Build MCKukaFRI" ON)
if(WITH_KUKA_FRI)
  list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
  find_package(KUKA_FRI REQUIRED)
endif()

find_program(XACRO xacro REQUIRED)
message(STATUS "Found xacro: ${XACRO}")

include(GNUInstallDirs)
set(INSTALL_DATADIR ${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME})

# xacro files used by both robots
set(xacro_DEPS
  ${IIWA_DESCRIPTION_PATH}/urdf/iiwa.gazebo.xacro
  ${IIWA_DESCRIPTION_PATH}/urdf/iiwa.transmission.xacro
  ${IIWA_DESCRIPTION_PATH}/urdf/materials.xacro
  ${IIWA_DESCRIPTION_PATH}/urdf/utilities.xacro
)

function(generate_iiwa_module MODEL)
  # Generates the URDF from the xacro files
  set(URDF_OUT ${CMAKE_CURRENT_BINARY_DIR}/urdf/${MODEL}.urdf)
  set(XACRO_IN ${IIWA_DESCRIPTION_PATH}/urdf/${MODEL}.urdf.xacro)
  add_custom_command(
    OUTPUT ${URDF_OUT}
    COMMAND ${XACRO} -o ${URDF_OUT} -i ${XACRO_IN}
    DEPENDS ${xacro_DEPS} ${XACRO_IN} ${IIWA_DESCRIPTION_PATH}/urdf/${MODEL}.xacro
    COMMENT "Generate ${MODEL}.urdf"
  )
  add_custom_target(generate_${MODEL}_model ALL DEPENDS ${URDF_OUT})
  install(FILES ${URDF_OUT} DESTINATION ${INSTALL_DATADIR}/urdf/)

  # Configure the mc_rtc alias
  set(ALIAS_OUT ${CMAKE_CURRENT_BINARY_DIR}/aliases/${MODEL}.yaml)
  configure_file(aliases/iiwa.in.yaml ${ALIAS_OUT} @ONLY)
  install(FILES ${ALIAS_OUT} DESTINATION ${MC_ROBOTS_ALIASES_DIRECTORY})
endfunction()

generate_iiwa_module(iiwa7)
generate_iiwa_module(iiwa14)
