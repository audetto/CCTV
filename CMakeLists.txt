cmake_minimum_required(VERSION 3.3)

project(cctv)

add_executable(cctv
  main.cpp
  dvr.cpp
  options.cpp
  utils.cpp
  )

set(SDK "$ENV{HOME}/projects/cctv/sdk")

set(SDK_INC "${SDK}/incEn")
set(SDK_LIB "${SDK}/lib")
set(SDK_COM "${SDK_LIB}/HCNetSDKCom")

target_include_directories(cctv PRIVATE ${SDK}/incEn)

find_library(HCNETSDK NAMES hcnetsdk PATHS ${SDK_LIB})
find_package(Boost REQUIRED
  COMPONENTS program_options
  )

target_link_libraries(cctv PRIVATE
  ${HCNETSDK}
  Boost::program_options
  )

target_compile_features(cctv PUBLIC cxx_std_17)

MESSAGE("CMAKE_BUILD_TYPE:                ${CMAKE_BUILD_TYPE}")
MESSAGE("CMAKE_CXX_FLAGS:                 ${CMAKE_CXX_FLAGS}")
MESSAGE("CMAKE_CXX_FLAGS_RELEASE:         ${CMAKE_CXX_FLAGS_RELEASE}")
MESSAGE("CMAKE_CXX_FLAGS_DEBUG:           ${CMAKE_CXX_FLAGS_DEBUG}")
MESSAGE("CMAKE_CXX_FLAGS_RELWIHDEBINFO:   ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")

add_custom_command(TARGET cctv POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${SDK_COM} "${CMAKE_BINARY_DIR}/HCNetSDKCom")
