#
# Copyright (c) 2017-2018 Bitprim Inc.
#
# This file is part of Bitprim.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

cmake_minimum_required(VERSION 3.4)

# bitprim-rpc
#==============================================================================
project(bitprim-rpc
        VERSION 0
        LANGUAGES CXX C)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Check for baseline language coverage in the compiler for the C++11 standard.
#------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 14)
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

set(BITPRIM_PROJECT_VERSION "-" CACHE STRING "Specify the Bitprim Project Version.")
# message(${BITPRIM_PROJECT_VERSION})


set(MICROARCHITECTURE "x86_64" CACHE STRING "Specify the Cryptocurrency (x86_64|...).")
message( STATUS "Bitprim: Compiling for ${MICROARCHITECTURE}")
# add_definitions(-DBITPRIM_MICROARCHITECTURE=${MICROARCHITECTURE})
# add_definitions(-DBITPRIM_MICROARCHITECTURE_STR="${MICROARCHITECTURE}")

# Implement --with-keoken and declare WITH_KEOKEN.
#------------------------------------------------------------------------------
option(WITH_KEOKEN "Keoken enabled." OFF)
if (WITH_KEOKEN)
  message(STATUS "Bitprim: Keoken enabled")
  add_definitions(-DBITPRIM_WITH_KEOKEN)
endif()

# Implement --with-mempool and declare WITH_MEMPOOL.
#------------------------------------------------------------------------------
option(WITH_MEMPOOL "Mempool enabled." OFF)
if (WITH_MEMPOOL)
  message(STATUS "Bitprim: Mempool enabled")
  add_definitions(-DBITPRIM_WITH_MEMPOOL)
endif()

# Implement --use-domain and declare USE_DOMAIN.
#------------------------------------------------------------------------------
option(USE_DOMAIN "Domain enabled." OFF)
if (USE_DOMAIN)
  message(STATUS "Bitprim: Using Domain enabled")
  add_definitions(-DBITPRIM_USE_DOMAIN)
endif()

# Implement and declare DB_TRANSACTION_UNCONFIRMED.
#------------------------------------------------------------------------------
option(DB_TRANSACTION_UNCONFIRMED "DB Transactions Unconfirmed enabled." OFF)
if (DB_TRANSACTION_UNCONFIRMED)
  message(STATUS "Bitprim: DB Transactions Unconfirmed enabled")
  add_definitions(-DBITPRIM_DB_TRANSACTION_UNCONFIRMED)
endif()

# Implement and declare DB_SPENDS.
#------------------------------------------------------------------------------
option(DB_SPENDS "DB Spends enabled." OFF)
if (DB_SPENDS)
  message(STATUS "Bitprim: DB Spends enabled")
  add_definitions(-DBITPRIM_DB_SPENDS)
endif()

# Implement and declare DB_HISTORY.
#------------------------------------------------------------------------------
option(DB_HISTORY "DB History enabled." OFF)
if (DB_HISTORY)
  message(STATUS "Bitprim: DB History enabled")
  add_definitions(-DBITPRIM_DB_HISTORY)
endif()

# Implement and declare DB_STEALTH.
#------------------------------------------------------------------------------
option(DB_STEALTH "DB Stealth enabled." OFF)
if (DB_STEALTH)
  message(STATUS "Bitprim: DB Stealth enabled")
  add_definitions(-DBITPRIM_DB_STEALTH)
endif()

# Implement and declare DB_UNSPENT_LIBBITCOIN.
#------------------------------------------------------------------------------
option(DB_UNSPENT_LIBBITCOIN "DB Unspent Libbitcoin enabled." OFF)
if (DB_UNSPENT_LIBBITCOIN)
  message(STATUS "Bitprim: DB Unspent Libbitcoin enabled")
  add_definitions(-DBITPRIM_DB_UNSPENT_LIBBITCOIN)
endif()

# Implement and declare DB_LEGACY.
#------------------------------------------------------------------------------
option(DB_LEGACY "DB Legacy enabled." OFF)
if (DB_LEGACY)
  message(STATUS "Bitprim: DB Legacy enabled")
  add_definitions(-DBITPRIM_DB_LEGACY)
endif()

# Implement and declare DB_NEW.
#------------------------------------------------------------------------------
option(DB_NEW "DB New enabled." OFF)
if (DB_NEW)
  message(STATUS "Bitprim: DB New enabled")
  add_definitions(-DBITPRIM_DB_NEW)
endif()

# Implement and declare DB_NEW_BLOCKS.
#------------------------------------------------------------------------------
option(DB_NEW_BLOCKS "DB Blocks New enabled." OFF)
if (DB_NEW_BLOCKS)
  message(STATUS "Bitprim: DB Blocks New enabled")
  add_definitions(-DBITPRIM_DB_NEW_BLOCKS)
endif()

# Implement and declare DB_NEW_FULL.
#------------------------------------------------------------------------------
option(DB_NEW_FULL "DB New Full enabled." OFF)
if (DB_NEW_FULL)
  message(STATUS "Bitprim: DB New Full enabled")
  add_definitions(-DBITPRIM_DB_NEW_FULL)
endif()

set(CURRENCY "BCH" CACHE STRING "Specify the Cryptocurrency (BCH|BTC|LTC).")

if (${CURRENCY} STREQUAL "BCH")
  add_definitions(-DBITPRIM_CURRENCY_BCH)
elseif (${CURRENCY} STREQUAL "BTC")
  add_definitions(-DBITPRIM_CURRENCY_BTC)
elseif (${CURRENCY} STREQUAL "LTC")
  add_definitions(-DBITPRIM_CURRENCY_LTC)
else()
  message(FATAL_ERROR "Invalid Cryptocurrency: ${CURRENCY}")
endif()

message(STATUS "Bitprim: Cryptocurrency: ${CURRENCY}")


# Conan infrastructure
#==============================================================================
if(EXISTS ${CMAKE_SOURCE_DIR}/ci_utils/cmake/bitprimbuildinfo.cmake)
    include(${CMAKE_SOURCE_DIR}/ci_utils/cmake/bitprimbuildinfo.cmake)
else()
    message( STATUS "bitprimbuildinfo doent exists")
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
  # Require bitprim-node of at least version 0 and output ${bitprim_rpc_CPPFLAGS/LIBS/PKG}.
  #------------------------------------------------------------------------------
  if (NOT TARGET bitprim-node)
    find_package(bitprim-node 0 REQUIRED)
  endif()
endif()


# Set flags.
#==============================================================================
include(CheckCXXCompilerFlag)

# Including common functions
#------------------------------------------------------------------------------
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/ci_utils/cmake)

include(BitprimTools)


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

# src/bitprim-rpc.la => ${libdir}
#------------------------------------------------------------------------------
set(MODE STATIC)
if (ENABLE_SHARED)
  set(MODE SHARED)
endif()

add_library(bitprim-rpc ${MODE}
    src/messages/utils.cpp
    src/http/rpc_server.cpp
    src/zmq/zmq_helper.cpp
    src/manager.cpp
)

if (ENABLE_POSITION_INDEPENDENT_CODE) 
  set_property(TARGET bitprim-rpc PROPERTY POSITION_INDEPENDENT_CODE ON)
endif(ENABLE_POSITION_INDEPENDENT_CODE)

  
target_include_directories(bitprim-rpc PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)


target_compile_definitions(bitprim-rpc PUBLIC -DBITPRIM_PROJECT_VERSION="\\"${BITPRIM_PROJECT_VERSION}\\"") #TODO(fernando): manage with Conan????

if (NOT ENABLE_SHARED)
  target_compile_definitions(bitprim-rpc PUBLIC -DBCT_STATIC -DBCN_STATIC -DBCR_STATIC -DBCB_STATIC -DBCD_STATIC -DBC_STATIC)
endif()

if (NOT MSVC)
  target_compile_definitions(bitprim-rpc PUBLIC -DSYSCONFDIR=\"${SYSCONFDIR}\")
endif()

if (NOT USE_CONAN)
    target_link_libraries(bitprim-rpc PUBLIC bitprim-node)
endif()

if (NOT NO_CONAN_AT_ALL)
  target_link_libraries(bitprim-rpc PUBLIC ${CONAN_LIBS})
elseif()
  target_include_directories(bitprim-rpc PUBLIC ${ZeroMQ_INCLUDE_DIR})
  target_link_libraries(bitprim-rpc PUBLIC ${ZeroMQ_LIBRARIES})
endif()

if (USE_CONAN)
  if (MINGW)
    target_link_libraries(bitprim-rpc PUBLIC ws2_32 wsock32) #TODO(fernando): manage with Conan
  endif()
endif()

_group_sources(bitprim-rpc "${CMAKE_CURRENT_LIST_DIR}")

# Tests
#==============================================================================

if (WITH_TESTS)
    enable_testing()
endif()

# local: test/bitprim_rpc_test
#------------------------------------------------------------------------------
if (WITH_TESTS)

  add_custom_target(tests ALL
      COMMAND ${CMAKE_CTEST_COMMAND} -C Debug --output-on-failure -R "test.+"
      COMMENT "Build and run all the unit tests.")

  add_executable(bitprim_rpc_test
          test/rpc.cpp
          test/user_agent_dummy.cpp
  )

  target_link_libraries(bitprim_rpc_test PUBLIC bitprim-rpc)

  # _group_sources(bitprim_rpc_test "${CMAKE_CURRENT_LIST_DIR}/test")

  add_test(NAME bitprim_rpc_test COMMAND bitprim_rpc_test)

  add_dependencies(tests bitprim_rpc_test)



#  add_executable(bitprim_rpc_test
#          test/rpc.cpp
#  )

#  target_link_libraries(bitprim_rpc_test PUBLIC bitprim-rpc)
#  _group_sources(bitprim_rpc_test "${CMAKE_CURRENT_LIST_DIR}/test")

#  _add_tests(bitprim_rpc_test
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
  target_link_libraries(rpc.bn bitprim-rpc)
  set_target_properties(
          rpc.bn PROPERTIES
          FOLDER "rpc"
          OUTPUT_NAME rpc)

endif (WITH_CONSOLE)


# Install
#==============================================================================
install(TARGETS bitprim-rpc
        EXPORT bitprim-rpc
        ARCHIVE DESTINATION lib)

set(_bitprim_headers
        bitprim/rpc.hpp
        bitprim/rpc/define.hpp
        bitprim/rpc/version.hpp
        bitprim/rpc/http/server_http.hpp
        bitprim/rpc/http/rpc_server.hpp
        bitprim/rpc/json/json.hpp
        bitprim/rpc/zmq/zmq_helper.hpp
        bitprim/rpc/messages.hpp
        bitprim/rpc/messages/messages.hpp
        bitprim/rpc/messages/blockchain/getrawtransaction.hpp
        bitprim/rpc/messages/blockchain/getaddressbalance.hpp
        bitprim/rpc/messages/blockchain/getspentinfo.hpp
        bitprim/rpc/messages/blockchain/getaddresstxids.hpp
        bitprim/rpc/messages/blockchain/getaddressdeltas.hpp
        bitprim/rpc/messages/blockchain/getaddressutxos.hpp
        bitprim/rpc/messages/blockchain/getblockhashes.hpp
        bitprim/rpc/messages/blockchain/getbestblockhash.hpp
        bitprim/rpc/messages/blockchain/getblock.hpp
        bitprim/rpc/messages/blockchain/getblockhash.hpp
        bitprim/rpc/messages/blockchain/getblockchaininfo.hpp
        bitprim/rpc/messages/blockchain/getblockheader.hpp
        bitprim/rpc/messages/blockchain/getblockcount.hpp
        bitprim/rpc/messages/blockchain/getdifficulty.hpp
        bitprim/rpc/messages/blockchain/getchaintips.hpp
        bitprim/rpc/messages/blockchain/getaddressmempool.hpp
        
        bitprim/rpc/messages/mining/getmininginfo.hpp
        bitprim/rpc/messages/util/getinfo.hpp
        bitprim/rpc/messages/util/getnetworkinfo.hpp
        bitprim/rpc/messages/util/validateaddress.hpp
        bitprim/rpc/messages/utils.hpp
        bitprim/rpc/messages/error_codes.hpp

        bitprim/rpc/messages/mining/submitblock.hpp
        bitprim/rpc/messages/wallet/sendrawtransaction.hpp

)

if (WITH_MEMPOOL)
    set(_bitprim_headers
        ${_bitprim_headers}
        bitprim/rpc/messages/mining/getblocktemplate.hpp
    )
endif()

foreach (_header ${_bitprim_headers})
  get_filename_component(_directory "${_header}" DIRECTORY)
  install(FILES "include/${_header}" DESTINATION "include/${_directory}")
endforeach()

# Export
#==============================================================================
include(CMakePackageConfigHelpers)

export(EXPORT bitprim-rpc
        FILE "${CMAKE_CURRENT_BINARY_DIR}/bitprim-rpcTargets.cmake")
export(PACKAGE bitprim-rpc)

if (UNIX)
  set(_config_package_location "lib/cmake")
elseif (WIN32)
  set(_config_package_location "cmake")
endif()

install(EXPORT bitprim-rpc
        FILE bitprim-rpcTargets.cmake
        DESTINATION ${_config_package_location})

configure_file(bitprim-rpcConfig.cmake.in
        "${CMAKE_CURRENT_BINARY_DIR}/bitprim-rpcConfig.cmake"
        @ONLY)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/bitprim-rpcConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion)

install(
        FILES
        "${CMAKE_CURRENT_BINARY_DIR}/bitprim-rpcConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/bitprim-rpcConfigVersion.cmake"
        DESTINATION ${_config_package_location})
