#
# Copyright 2023 mc_rtc development team
#

# Try to find the KUKA FRI library
#
# This can use KUKA_FRI_PREFIX as an hint
#
# Defines the KUKA_FRI::KUKA_FRI target

set(TGT KUKA_FRI::KUKA_FRI)
if(TARGET ${TGT})
  return()
endif()

if(NOT DEFINED KUKA_FRI_PREFIX)
  set(KUKA_FRI_PREFIX ${CMAKE_INSTALL_PREFIX})
endif()

find_path(
  KUKA_FRI_INCLUDE_DIR
  NAMES kuka/fri/LBRState.h
  HINTS ${KUKA_FRI_PREFIX})
message(STATUS "Found KUKA_FRI directory: ${KUKA_FRI_INCLUDE_DIR}")

unset(KUKA_FRI_LIBRARY CACHE)
find_library(
  KUKA_FRI_LIBRARY
  NAMES kuka_fri
  PATHS ${KUKA_FRI_PREFIX})
message(STATUS "Found KUKA_FRI library: ${KUKA_FRI_LIBRARY}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(KUKA_FRI DEFAULT_MSG KUKA_FRI_LIBRARY
                                  KUKA_FRI_INCLUDE_DIR)
mark_as_advanced(KUKA_FRI_INCLUDE_DIR KUKA_FRI_LIBRARY)

if(KUKA_FRI_FOUND)
  add_library(${TGT} INTERFACE IMPORTED GLOBAL)
  set_target_properties(
    ${TGT} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${KUKA_FRI_INCLUDE_DIR}
                      INTERFACE_LINK_LIBRARIES ${KUKA_FRI_LIBRARY})
endif()
