cmake_minimum_required(VERSION 3.4)

# bitprim-rpc
#==============================================================================
project(bitprim-rpc
        VERSION 0.4.0
        LANGUAGES CXX C)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  if (NOT NOT_USE_CPP11_ABI)
    add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1)
  else()
    add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
  endif()
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-macro-redefined")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Check for baseline language coverage in the compiler for the C++11 standard.
#------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 11)
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

# # Implement --with-litecoin.
# #------------------------------------------------------------------------------
# option(WITH_LITECOIN "Compile with Litecoin support." OFF)


set(BITPRIM_BUILD_NUMBER "-" CACHE STRING "Specify the Bitprim Build Number.")
# message(${BITPRIM_BUILD_NUMBER})


# Conan infrastructure.
#==============================================================================
# Only if enabled
#------------------------------------------------------------------------------
if (NOT NO_CONAN_AT_ALL)
  if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
      include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
      conan_basic_setup()
  else()
      message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
  endif()
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
endif()


if (NOT USE_CONAN)
  # Require bitprim-blockchain of at least version 0.3.0 and output ${bitprim_rpc_CPPFLAGS/LIBS/PKG}.
  #------------------------------------------------------------------------------
  if (NOT TARGET bitprim-blockchain)
    find_package(bitprim-blockchain 0.3.0 REQUIRED)
  endif()

  # Require zmq of at least version 4.1.1 and output ${zmq_CPPFLAGS/LIBS/PKG}.
  #------------------------------------------------------------------------------
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
  find_package(ZeroMQ 4.1.1 REQUIRED)
endif()


# if (WITH_LITECOIN)
#   add_definitions(-DLITECOIN)
# endif()

# Set flags.
#==============================================================================
include(CheckCXXCompilerFlag)

# Including common functions
#------------------------------------------------------------------------------
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
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
if (MSVC)
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
    src/messages.cpp
    src/rpc_server.cpp
    src/messages/blockchain/getrawtransaction.cpp
    src/messages/blockchain/getaddressbalance.cpp
    src/messages/blockchain/getspentinfo.cpp
    src/messages/blockchain/getaddresstxids.cpp
    src/messages/blockchain/getaddressdeltas.cpp
    src/messages/blockchain/getaddressutxos.cpp
    src/messages/blockchain/getblockhashes.cpp
    src/messages/blockchain/getbestblockhash.cpp
    src/messages/blockchain/getblock.cpp
    src/messages/blockchain/getblockhash.cpp
    src/messages/blockchain/getblockchaininfo.cpp
    src/messages/blockchain/getblockheader.cpp
    src/messages/blockchain/getblockcount.cpp
    src/messages/blockchain/getdifficulty.cpp
    src/messages/blockchain/getchaintips.cpp
    src/messages/mining/getblocktemplate.cpp
    src/messages/mining/submitblock.cpp
    src/messages/mining/getmininginfo.cpp
    src/messages/wallet/sendrawtransaction.cpp
    src/messages/util/getinfo.cpp
    src/messages/util/validateaddress.cpp
    src/messages/utils.cpp
)

if (ENABLE_POSITION_INDEPENDENT_CODE) 
  set_property(TARGET bitprim-rpc PROPERTY POSITION_INDEPENDENT_CODE ON)
endif(ENABLE_POSITION_INDEPENDENT_CODE)

  
target_include_directories(bitprim-rpc PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)


target_compile_definitions(bitprim-rpc PUBLIC -DBITPRIM_BUILD_NUMBER="\\"${BITPRIM_BUILD_NUMBER}\\"")

if (NOT ENABLE_SHARED)
  target_compile_definitions(bitprim-rpc PUBLIC -DBCR_STATIC)
endif()

if (NOT MSVC)
  target_compile_definitions(bitprim-rpc PUBLIC -DSYSCONFDIR=\"${SYSCONFDIR}\")
endif()

if (NOT USE_CONAN)
    target_link_libraries(bitprim-rpc PUBLIC bitprim-blockchain)
    target_include_directories(bitprim-rpc PUBLIC ${ZeroMQ_INCLUDE_DIR})
    target_link_libraries(bitprim-rpc PUBLIC ${ZeroMQ_LIBRARIES})
endif()

if (NOT NO_CONAN_AT_ALL)
  target_link_libraries(bitprim-rpc PUBLIC ${CONAN_LIBS})
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
  add_executable(bitprim_rpc_test
          test/utility.hpp
  )

  target_link_libraries(bitprim_rpc_test PUBLIC bitprim-rpc)
  _group_sources(bitprim_rpc_test "${CMAKE_CURRENT_LIST_DIR}/test")

  _add_tests(bitprim_rpc_test
          configuration_tests
          node_tests
          header_queue_tests
          performance_tests
          reservation_tests
          reservations_tests
          settings_tests)
endif()


# Install
#==============================================================================
install(TARGETS bitprim-rpc
        EXPORT bitprim-rpc
        ARCHIVE DESTINATION lib)

set(_bitprim_headers
        bitprim/rpc.hpp
        bitprim/rpc/define.hpp
        bitprim/rpc/version.hpp
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
        bitprim/rpc/messages/mining/getblocktemplate.hpp
        bitprim/rpc/messages/mining/submitblock.hpp
        bitprim/rpc/messages/mining/getmininginfo.hpp
        bitprim/rpc/messages/wallet/sendrawtransaction.hpp
        bitprim/rpc/messages/util/getinfo.hpp
        bitprim/rpc/messages/util/validateaddress.hpp
        bitprim/rpc/messages/utils.hpp
)

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
