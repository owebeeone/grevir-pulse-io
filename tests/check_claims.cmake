set(include_args)
foreach(dir IN LISTS INCLUDE_DIRS)
  if(NOT dir STREQUAL "")
    list(APPEND include_args "-I${dir}")
  endif()
endforeach()
execute_process(COMMAND "${CXX}" -std=c++23 -DHAS_STD_LIB=1 ${include_args}
  -fsyntax-only "${SOURCE}"
  RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE errors)
if(result EQUAL 0 OR NOT errors MATCHES "Application has resource conflict")
  message(FATAL_ERROR "Duplicate GPIO claim did not fail as expected: ${output}${errors}")
endif()
message(STATUS "Pulse IO duplicate GPIO claim rejected")
