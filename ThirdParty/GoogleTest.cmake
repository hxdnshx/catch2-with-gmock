# https://github.com/matepek/catch2-with-gmock

include(FetchContent)

# Prevent overriding the parent project's compiler/linker settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(googletest
                     GIT_REPOSITORY https://github.com/google/googletest.git
                     GIT_TAG release-1.12.1)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR}
                   EXCLUDE_FROM_ALL)
endif()

mark_as_advanced(BUILD_GMOCK
                 BUILD_GTEST
                 BUILD_SHARED_LIBS
                 gmock_build_tests
                 gtest_build_samples
                 gtest_build_tests
                 gtest_disable_pthreads
                 gtest_force_shared_crt
                 gtest_hide_internal_symbols)

set_target_properties(gtest PROPERTIES FOLDER extern)
set_target_properties(gtest_main PROPERTIES FOLDER extern)
set_target_properties(gmock PROPERTIES FOLDER extern)
set_target_properties(gmock_main PROPERTIES FOLDER extern)

#

add_library(ThirdParty.GoogleMock INTERFACE)

target_link_libraries(ThirdParty.GoogleMock INTERFACE gtest gmock)

target_include_directories(
  ThirdParty.GoogleMock
  INTERFACE "${googletest_SOURCE_DIR}/googlemock/include"
            "${googletest_SOURCE_DIR}/googletest/include")

#

function(add_gtest_with_main TESTNAME)
  add_executable(${TESTNAME} ${ARGN})
  target_link_libraries(${TESTNAME} PRIVATE ThirdParty.GoogleMockWithMain)
  add_test(NAME "${TESTNAME}" COMMAND "${TESTNAME}")
endfunction()
