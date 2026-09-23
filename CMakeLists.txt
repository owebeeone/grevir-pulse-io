cmake_minimum_required(VERSION 3.20)
project(grevir_pulse_io VERSION 0.1.0 LANGUAGES CXX)

foreach(dependency IN ITEMS base time core peripherals pulse-codec)
  string(REPLACE "-" "_" target_name "${dependency}")
  if(NOT TARGET grevir::${target_name})
    find_package(grevir-${dependency} CONFIG REQUIRED)
  endif()
endforeach()

add_library(grevir_pulse_io INTERFACE)
add_library(grevir::pulse_io ALIAS grevir_pulse_io)
set_target_properties(grevir_pulse_io PROPERTIES EXPORT_NAME pulse_io)
target_compile_features(grevir_pulse_io INTERFACE cxx_std_23)
target_link_libraries(grevir_pulse_io INTERFACE
  grevir::base grevir::time grevir::core grevir::peripherals grevir::pulse_codec)
target_include_directories(grevir_pulse_io INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include>)

include(GNUInstallDirs)
install(TARGETS grevir_pulse_io EXPORT GrevirPulseIOTargets)
install(DIRECTORY src/ DESTINATION include FILES_MATCHING
  PATTERN "*.hpp" PATTERN "*.h")
install(EXPORT GrevirPulseIOTargets NAMESPACE grevir::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/grevir-pulse-io)
install(FILES cmake/grevir-pulse-io-config.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/grevir-pulse-io)
install(FILES LICENSE.txt DESTINATION ${CMAKE_INSTALL_DATADIR}/grevir-pulse-io)

option(GREVIR_BUILD_COMPILE_CHECKS "Compile native headers and template users" OFF)
option(GREVIR_BUILD_HOST_TESTS "Run deterministic native behavior tests" OFF)
if(GREVIR_BUILD_HOST_TESTS)
  enable_testing()
endif()
if(GREVIR_BUILD_COMPILE_CHECKS OR GREVIR_BUILD_HOST_TESTS)
  add_subdirectory(tests)
endif()
