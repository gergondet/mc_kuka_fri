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

find_program(MESH_SAMPLING mesh_sampling REQUIRED)
message(STATUS "Found mesh_sampling: ${MESH_SAMPLING}")

find_program(QCONVEX qconvex REQUIRED)
message(STATUS "Found qconvex: ${QCONVEX}")

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

  # Generate convex files
  set(IIWA_LINKS
    link_0
    link_1
    link_2
    link_3
    link_4
    link_5
    link_6
    link_7
  )
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cloud/${MODEL}/)
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/convex/${MODEL}/)
  foreach(LINK ${IIWA_LINKS})
    set(MESH_IN ${IIWA_DESCRIPTION_PATH}/meshes/${MODEL}/visual/${LINK}.stl)
    set(CLOUD_OUT ${CMAKE_CURRENT_BINARY_DIR}/cloud/${MODEL}/${LINK}.qc)
    set(CONVEX_OUT ${CMAKE_CURRENT_BINARY_DIR}/convex/${MODEL}/iiwa_${LINK}-ch.txt)
    add_custom_command(OUTPUT ${CONVEX_OUT}
      COMMAND ${MESH_SAMPLING} ${MESH_IN} ${CLOUD_OUT} --type xyz --samples 1000
      COMMAND ${QCONVEX} TI ${CLOUD_OUT} TO ${CONVEX_OUT} Qt o f
      DEPENDS ${MESH_IN}
      COMMENT "Generate ${CONVEX_OUT}"
    )
    add_custom_target(generate_${MODEL}_${LINK}_convex ALL DEPENDS ${CONVEX_OUT})
    install(FILES ${CONVEX_OUT} DESTINATION ${INSTALL_DATADIR}/convex/${MODEL}/)
  endforeach()
endfunction()

generate_iiwa_module(iiwa7)
generate_iiwa_module(iiwa14)
