# https://github.com/matepek/catch2-with-gmock

include(FetchContent)

FetchContent_Declare(easymock
                     GIT_REPOSITORY https://github.com/hxdnshx/easymock.git)

FetchContent_GetProperties(easymock)
if(NOT easymock_POPULATED)
  FetchContent_Populate(easymock)
endif()

#

add_library(ThirdParty.easymock INTERFACE)

target_include_directories(ThirdParty.easymock INTERFACE "${easymock_SOURCE_DIR}")

target_compile_features(ThirdParty.easymock INTERFACE cxx_std_11)

#
# Header only library.
