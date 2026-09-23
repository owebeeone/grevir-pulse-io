if(GREVIR_BUILD_COMPILE_CHECKS)
  file(GLOB_RECURSE public_headers CONFIGURE_DEPENDS RELATIVE
    "${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/src/*.hpp"
    "${PROJECT_SOURCE_DIR}/src/*.h")
  set(header_sources)
  foreach(header IN LISTS public_headers)
    string(MAKE_C_IDENTIFIER "${header}" identifier)
    set(source "${CMAKE_CURRENT_BINARY_DIR}/${identifier}.cpp")
    file(WRITE "${source}" "#include <${header}>\n")
    list(APPEND header_sources "${source}")
  endforeach()
  add_library(grevir_pulse_io_compile OBJECT native_compile.cpp ${header_sources})
  target_link_libraries(grevir_pulse_io_compile PRIVATE grevir::pulse_io)
  set_target_properties(grevir_pulse_io_compile PROPERTIES CXX_EXTENSIONS OFF)
  if(CMAKE_CXX_COMPILER_ID MATCHES "^(AppleClang|Clang|GNU)$")
    add_custom_target(grevir_pulse_io_claim_checks ALL
      COMMAND "${CMAKE_COMMAND}"
        "-DCXX=${CMAKE_CXX_COMPILER}"
        "-DINCLUDE_DIRS=$<TARGET_PROPERTY:grevir_pulse_io_compile,INCLUDE_DIRECTORIES>"
        "-DSOURCE=${CMAKE_CURRENT_SOURCE_DIR}/conflicting_pin_probe.cpp"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/check_claims.cmake"
      VERBATIM)
  endif()
endif()

if(GREVIR_BUILD_HOST_TESTS)
  if(NOT COMMAND catch_discover_tests)
    find_package(grevir-test-support CONFIG REQUIRED)
  endif()
  add_executable(grevir_pulse_io_runtime module_test.cpp)
  target_link_libraries(grevir_pulse_io_runtime PRIVATE
    grevir::pulse_io Catch2::Catch2WithMain)
  set_target_properties(grevir_pulse_io_runtime PROPERTIES CXX_EXTENSIONS OFF)
  catch_discover_tests(grevir_pulse_io_runtime TEST_PREFIX "pulse-io."
    PROPERTIES LABELS "pulse-io" TIMEOUT 10)
endif()
