# Copyright (c) 2016-2020 Knuth Project developers.
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

cmake_minimum_required(VERSION 3.8)

# kth-rpc
#==============================================================================
project(kth-rpc
        VERSION 0
        LANGUAGES CXX C)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Check for baseline language coverage in the compiler for the C++17 standard.
#------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Process options.
#==============================================================================

# Implement --use-conan
#------------------------------------------------------------------------------
option(USE_CONAN "Use Conan Build Tool." OFF)
option(NO_CONAN_AT_ALL "Conan totally disabled." OFF)

if (NO_CONAN_AT_ALL)
  set(USE_CONAN OFF)
endif()


# Inherit --enable-shared and define BOOST_TEST_DYN_LINK.
#------------------------------------------------------------------------------
option(ENABLE_SHARED "" OFF)

# Implement POSITION_INDEPENDENT_CODE
#------------------------------------------------------------------------------
option(ENABLE_POSITION_INDEPENDENT_CODE "Enable POSITION_INDEPENDENT_CODE property" ON)

# Implement --with-tests and declare WITH_TESTS.
#------------------------------------------------------------------------------
option(WITH_TESTS "Compile with unit tests." ON)

# Implement --with-console and declare WITH_CONSOLE.
#------------------------------------------------------------------------------
option(WITH_CONSOLE "Compile console application." ON)




set(KTH_PROJECT_VERSION "-" CACHE STRING "Specify the Knuth Project Version.")
# message(${KTH_PROJECT_VERSION})
message( STATUS "Knuth: Compiling version ${KTH_PROJECT_VERSION}")

set(MICROARCHITECTURE "x86_64" CACHE STRING "Specify the Microarchitecture (x86_64|...).")
message( STATUS "Knuth: Compiling for ${MICROARCHITECTURE}")
# add_definitions(-DKTH_MICROARCHITECTURE=${MICROARCHITECTURE})
# add_definitions(-DKTH_MICROARCHITECTURE_STR="${MICROARCHITECTURE}")

set(MARCH_ID "" CACHE STRING "Specify the Microarchitecture ID (x86_64|...).")
message( STATUS "Knuth: Compiling for Microarchitecture ID ${MARCH_ID}")

message( STATUS "Knuth: CONAN_CXX_FLAGS ${CONAN_CXX_FLAGS}")
message( STATUS "Knuth: CONAN_C_FLAGS ${CONAN_C_FLAGS}")



# Implement --with-keoken and declare WITH_KEOKEN.
#------------------------------------------------------------------------------
option(WITH_KEOKEN "Keoken enabled." OFF)
if (WITH_KEOKEN)
  message(STATUS "Knuth: Keoken enabled")
  add_definitions(-DKTH_WITH_KEOKEN)
endif()

# Implement --with-mempool and declare WITH_MEMPOOL.
#------------------------------------------------------------------------------
option(WITH_MEMPOOL "Mempool enabled." OFF)
if (WITH_MEMPOOL)
  message(STATUS "Knuth: Mempool enabled")
  add_definitions(-DKTH_WITH_MEMPOOL)
endif()

# Implement and declare DB_TRANSACTION_UNCONFIRMED.
#------------------------------------------------------------------------------
option(DB_TRANSACTION_UNCONFIRMED "DB Transactions Unconfirmed enabled." OFF)
if (DB_TRANSACTION_UNCONFIRMED)
  message(STATUS "Knuth: DB Transactions Unconfirmed enabled")
  add_definitions(-DKTH_DB_TRANSACTION_UNCONFIRMED)
endif()

# Implement and declare DB_SPENDS.
#------------------------------------------------------------------------------
option(DB_SPENDS "DB Spends enabled." OFF)
if (DB_SPENDS)
  message(STATUS "Knuth: DB Spends enabled")
  add_definitions(-DKTH_DB_SPENDS)
endif()

# Implement and declare DB_HISTORY.
#------------------------------------------------------------------------------
option(DB_HISTORY "DB History enabled." OFF)
if (DB_HISTORY)
  message(STATUS "Knuth: DB History enabled")
  add_definitions(-DKTH_DB_HISTORY)
endif()

# Implement and declare DB_STEALTH.
#------------------------------------------------------------------------------
option(DB_STEALTH "DB Stealth enabled." OFF)
if (DB_STEALTH)
  message(STATUS "Knuth: DB Stealth enabled")
  add_definitions(-DKTH_DB_STEALTH)
endif()

# Implement and declare DB_UNSPENT_LEGACY.
#------------------------------------------------------------------------------
option(DB_UNSPENT_LEGACY "DB Unspent Legacy enabled." OFF)
if (DB_UNSPENT_LEGACY)
  message(STATUS "Knuth: DB Unspent Legacy enabled")
  add_definitions(-DKTH_DB_UNSPENT_LEGACY)
endif()

# Implement and declare DB_LEGACY.
#------------------------------------------------------------------------------
option(DB_LEGACY "DB Legacy enabled." OFF)
if (DB_LEGACY)
  message(STATUS "Knuth: DB Legacy enabled")
  add_definitions(-DKTH_DB_LEGACY)
endif()

# Implement and declare DB_NEW.
#------------------------------------------------------------------------------
option(DB_NEW "DB New enabled." OFF)
if (DB_NEW)
  message(STATUS "Knuth: DB New enabled")
  add_definitions(-DKTH_DB_NEW)
endif()

# Implement and declare DB_NEW_BLOCKS.
#------------------------------------------------------------------------------
option(DB_NEW_BLOCKS "DB Blocks New enabled." OFF)
if (DB_NEW_BLOCKS)
  message(STATUS "Knuth: DB Blocks New enabled")
  add_definitions(-DKTH_DB_NEW_BLOCKS)
endif()

# Implement and declare DB_NEW_FULL.
#------------------------------------------------------------------------------
option(DB_NEW_FULL "DB New Full enabled." OFF)
if (DB_NEW_FULL)
  message(STATUS "Knuth: DB New Full enabled")
  add_definitions(-DKTH_DB_NEW_FULL)
endif()

# Implement --db_readonly_mode and declare KTH_DB_READONLY.
#------------------------------------------------------------------------------
option(DB_READONLY_MODE "Readonly DB mode enabled." OFF)
if (DB_READONLY_MODE)
  message(STATUS "Knuth: Readonly DB mode enabled")
  add_definitions(-DKTH_DB_READONLY)
endif()


# # Implement --binlog.
# #------------------------------------------------------------------------------
# option(BINLOG "Uses BinLog logging library." OFF)
# if (BINLOG)
#   add_definitions(-DKTH_USE_BINLOG)
# endif()

set(LOG_LIBRARY "boost" CACHE STRING "Setting for the logging library (boost|spdlog|binlog).")

if (${LOG_LIBRARY} STREQUAL "boost")
  add_definitions(-DKTH_LOG_LIBRARY_BOOST)
elseif (${LOG_LIBRARY} STREQUAL "spdlog")
  add_definitions(-DKTH_LOG_LIBRARY_SPDLOG)
elseif (${LOG_LIBRARY} STREQUAL "binlog")
  add_definitions(-DKTH_LOG_LIBRARY_BINLOG)
else()
  message(FATAL_ERROR "Invalid Logging Library: ${LOG_LIBRARY}")
endif()

message(STATUS "Knuth: Logging Library: ${LOG_LIBRARY}")


# Implement --use_libmdbx.
#------------------------------------------------------------------------------
option(USE_LIBMDBX "Uses libmdbx DB library." OFF)
if (USE_LIBMDBX)
  add_definitions(-DKTH_USE_LIBMDBX)
endif()



set(CURRENCY "BCH" CACHE STRING "Specify the Cryptocurrency (BCH|BTC|LTC).")

if (${CURRENCY} STREQUAL "BCH")
  add_definitions(-DKTH_CURRENCY_BCH)
elseif (${CURRENCY} STREQUAL "BTC")
  add_definitions(-DKTH_CURRENCY_BTC)
elseif (${CURRENCY} STREQUAL "LTC")
  add_definitions(-DKTH_CURRENCY_LTC)
else()
  message(FATAL_ERROR "Invalid Cryptocurrency: ${CURRENCY}")
endif()

message(STATUS "Knuth: Cryptocurrency: ${CURRENCY}")


# Conan infrastructure
#==============================================================================
if(EXISTS ${CMAKE_SOURCE_DIR}/ci_utils/cmake/knuthbuildinfo.cmake)
    include(${CMAKE_SOURCE_DIR}/ci_utils/cmake/knuthbuildinfo.cmake)
else()
    message( STATUS "knuthbuildinfo doent exists")
endif()



# Check dependencies.
#==============================================================================
#------------------------------------------------------------------------------
if (NO_CONAN_AT_ALL)
  # Require Boost of at least version 1.56.0 and output ${boost_CPPFLAGS/LDFLAGS}.
  #------------------------------------------------------------------------------
  if (NOT ENABLE_SHARED)
    set(Boost_USE_STATIC_LIBS ON)
  endif()

  if (MSVC)
    find_package(
      Boost 1.56.0 REQUIRED
      COMPONENTS atomic chrono date_time filesystem iostreams locale log log_setup program_options regex system thread unit_test_framework
    )
  else()
    find_package(
      Boost 1.56.0 REQUIRED
      COMPONENTS chrono date_time filesystem iostreams locale log program_options regex system thread unit_test_framework
    )
  endif()
  # Require zmq of at least version 4.1.1 and output ${zmq_CPPFLAGS/LIBS/PKG}.
  #------------------------------------------------------------------------------
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
  find_package(ZeroMQ 4.1.1 REQUIRED)
endif()


if (NOT USE_CONAN)
  # Require kth-node of at least version 0 and output ${kth_rpc_CPPFLAGS/LIBS/PKG}.
  #------------------------------------------------------------------------------
  if (NOT TARGET kth-node)
    find_package(kth-node 0 REQUIRED)
  endif()
endif()


# Set flags.
#==============================================================================
include(CheckCXXCompilerFlag)

# Including common functions
#------------------------------------------------------------------------------
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/ci_utils/cmake)

include(KnuthTools)


# Warn on all stuff.
#------------------------------------------------------------------------------
if (NOT MSVC)
  _add_c_compile_flag(-Wall _has_all_warning_flag)
else()
  _add_c_compile_flag(-W4 _has_all_warning_flag)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Warn on extra stuff.
#------------------------------------------------------------------------------
if (NOT MSVC)
  _add_c_compile_flag(-Wextra _has_extra_warning_flag)
endif()

# Be really annoying.
#------------------------------------------------------------------------------
_add_c_compile_flag(-Wpedantic _has_pedantic_warning_flag)
if (_has_pedantic_warning_flag)
  _add_c_compile_flag(-pedantic _has_pedantic_flag)
endif()

# Conform to style.
#------------------------------------------------------------------------------
_add_cxx_compile_flag(-Wno-missing-braces _has_no_missing_braces_warning_flag)

# Conflict in stdlib under clang. Enabled in clang only.
#------------------------------------------------------------------------------
_add_cxx_compile_flag(-Wno-mismatched-tags _has_no_mismatched_tags_warning_flag)

# Clean up boost 1.55 headers. Enabled in gcc only.
#------------------------------------------------------------------------------
_add_c_compile_flag(-Wno-deprecated-declarations _has_no_deprecated_declarations_warning_flag)

# Protect stack.
#------------------------------------------------------------------------------
_add_link_flag(-fstack-protector _has_stack_protector_flag)

# Protect stack comprehensively.
#------------------------------------------------------------------------------
_add_link_flag(-fstack-protector-all _has_stack_protector_all_flag)

# Hide internal functions from external libs. Enabled in gcc only.
#------------------------------------------------------------------------------
_add_cxx_compile_flag(-fvisibility-hidden _has_visibility_hidden_flag)

# Hide inlines from external libs. Enabled in gcc only.
#------------------------------------------------------------------------------
_add_cxx_compile_flag(-fvisibility-inlines-hidden _has_visibility_inlines_hidden_flag)

# Target Windows Vista. Enabled in msvc only.
#------------------------------------------------------------------------------
if (MSVC) # OR MINGW)
  add_definitions(-D_WIN32_WINNT=0x0600)
endif()

# Build
#==============================================================================

# src/kth-rpc.la => ${libdir}
#------------------------------------------------------------------------------
set(MODE STATIC)
if (ENABLE_SHARED)
  set(MODE SHARED)
endif()

add_library(kth-rpc ${MODE}
    src/messages/utils.cpp
    src/http/rpc_server.cpp
    src/zmq/zmq_helper.cpp
    src/manager.cpp
)

if (ENABLE_POSITION_INDEPENDENT_CODE) 
  set_property(TARGET kth-rpc PROPERTY POSITION_INDEPENDENT_CODE ON)
endif(ENABLE_POSITION_INDEPENDENT_CODE)

  
target_include_directories(kth-rpc PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)


# target_compile_definitions(kth-rpc PUBLIC -DKTH_PROJECT_VERSION="\\"${KTH_PROJECT_VERSION}\\"") #TODO(fernando): manage with Conan????
target_compile_definitions(kth-rpc PUBLIC -DKTH_PROJECT_VERSION="${KTH_PROJECT_VERSION}") #TODO(fernando): manage with Conan????


if (NOT ENABLE_SHARED)
  target_compile_definitions(kth-rpc PUBLIC -DBCT_STATIC -DBCN_STATIC -DBCR_STATIC -DBCB_STATIC -DBCD_STATIC -DBC_STATIC)
endif()

if (NOT MSVC)
  target_compile_definitions(kth-rpc PUBLIC -DSYSCONFDIR=\"${SYSCONFDIR}\")
endif()

if (NOT USE_CONAN)
    target_link_libraries(kth-rpc PUBLIC kth-node)
endif()

if (NOT NO_CONAN_AT_ALL)
  target_link_libraries(kth-rpc PUBLIC ${CONAN_LIBS})
elseif()
  target_include_directories(kth-rpc PUBLIC ${ZeroMQ_INCLUDE_DIR})
  target_link_libraries(kth-rpc PUBLIC ${ZeroMQ_LIBRARIES})
endif()

if (USE_CONAN)
  if (MINGW)
    target_link_libraries(kth-rpc PUBLIC ws2_32 wsock32) #TODO(fernando): manage with Conan
  endif()
endif()

_group_sources(kth-rpc "${CMAKE_CURRENT_LIST_DIR}")

# Tests
#==============================================================================

if (WITH_TESTS)
    enable_testing()
endif()

# local: test/kth_rpc_test
#------------------------------------------------------------------------------
if (WITH_TESTS)

  # add_custom_target(tests ALL
  #     COMMAND ${CMAKE_CTEST_COMMAND} -C Debug --output-on-failure -R "test.+"
  #     COMMENT "Build and run all the unit tests.")

  add_executable(kth_rpc_test
          test/rpc.cpp
          test/user_agent_dummy.cpp
          test/main.cpp
  )

  target_link_libraries(kth_rpc_test PUBLIC kth-rpc)
  _group_sources(kth_rpc_test "${CMAKE_CURRENT_LIST_DIR}/test")

  target_include_directories(kth_rpc_test PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/test>)

  include(CTest)
  include(Catch)
  catch_discover_tests(kth_rpc_test)


  # add_dependencies(tests kth_rpc_test)
  # add_test(NAME kth_rpc_test COMMAND kth_rpc_test)

#  add_executable(kth_rpc_test
#          test/rpc.cpp
#  )

#  target_link_libraries(kth_rpc_test PUBLIC kth-rpc)
#  _group_sources(kth_rpc_test "${CMAKE_CURRENT_LIST_DIR}/test")

#  _add_tests(kth_rpc_test
#          configuration_tests
#          node_tests
#          header_queue_tests
#          performance_tests
#          reservation_tests
#          reservations_tests
#          settings_tests)
endif()


if (WITH_CONSOLE)

  add_executable(rpc.bn
          src/testing-main.cpp)
  target_link_libraries(rpc.bn kth-rpc)
  set_target_properties(
          rpc.bn PROPERTIES
          FOLDER "rpc"
          OUTPUT_NAME rpc)

endif (WITH_CONSOLE)


# Install
#==============================================================================
install(TARGETS kth-rpc
        EXPORT kth-rpc
        ARCHIVE DESTINATION lib)

set(_kth_headers
        kth/rpc.hpp
        kth/rpc/define.hpp
        kth/rpc/version.hpp
        kth/rpc/http/server_http.hpp
        kth/rpc/http/rpc_server.hpp
        kth/rpc/json/json.hpp
        kth/rpc/zmq/zmq_helper.hpp
        kth/rpc/messages.hpp
        kth/rpc/messages/messages.hpp
        kth/rpc/messages/blockchain/getrawtransaction.hpp
        kth/rpc/messages/blockchain/getaddressbalance.hpp
        kth/rpc/messages/blockchain/getspentinfo.hpp
        kth/rpc/messages/blockchain/getaddresstxids.hpp
        kth/rpc/messages/blockchain/getaddressdeltas.hpp
        kth/rpc/messages/blockchain/getaddressutxos.hpp
        kth/rpc/messages/blockchain/getblockhashes.hpp
        kth/rpc/messages/blockchain/getbestblockhash.hpp
        kth/rpc/messages/blockchain/getblock.hpp
        kth/rpc/messages/blockchain/getblockhash.hpp
        kth/rpc/messages/blockchain/getblockchaininfo.hpp
        kth/rpc/messages/blockchain/getblockheader.hpp
        kth/rpc/messages/blockchain/getblockcount.hpp
        kth/rpc/messages/blockchain/getdifficulty.hpp
        kth/rpc/messages/blockchain/getchaintips.hpp
        kth/rpc/messages/blockchain/getaddressmempool.hpp
        
        kth/rpc/messages/mining/getmininginfo.hpp
        kth/rpc/messages/util/getinfo.hpp
        kth/rpc/messages/util/getnetworkinfo.hpp
        kth/rpc/messages/util/validateaddress.hpp
        kth/rpc/messages/utils.hpp
        kth/rpc/messages/error_codes.hpp

        kth/rpc/messages/mining/submitblock.hpp
        kth/rpc/messages/wallet/sendrawtransaction.hpp

)

if (WITH_MEMPOOL)
    set(_kth_headers
        ${_kth_headers}
        kth/rpc/messages/mining/getblocktemplate.hpp
    )
endif()

foreach (_header ${_kth_headers})
  get_filename_component(_directory "${_header}" DIRECTORY)
  install(FILES "include/${_header}" DESTINATION "include/${_directory}")
endforeach()

# Export
#==============================================================================
include(CMakePackageConfigHelpers)

export(EXPORT kth-rpc
        FILE "${CMAKE_CURRENT_BINARY_DIR}/kth-rpcTargets.cmake")
export(PACKAGE kth-rpc)

if (UNIX)
  set(_config_package_location "lib/cmake")
elseif (WIN32)
  set(_config_package_location "cmake")
endif()

install(EXPORT kth-rpc
        FILE kth-rpcTargets.cmake
        DESTINATION ${_config_package_location})

configure_file(kth-rpcConfig.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/kth-rpcConfig.cmake"
        @ONLY)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/kth-rpcConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion)

install(
        FILES
        "${CMAKE_CURRENT_BINARY_DIR}/kth-rpcConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/kth-rpcConfigVersion.cmake"
        DESTINATION ${_config_package_location})
