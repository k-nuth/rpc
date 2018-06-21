/**
 * Copyright (c) 2017 Bitprim developers (see AUTHORS)
 *
 * This file is part of Bitprim.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BITPRIM_RPC_VERSION_HPP_
#define BITPRIM_RPC_VERSION_HPP_

/**
 * The semantic version of this repository as: [major].[minor].[patch]
 * For interpretation of the versioning scheme see: http://semver.org
 */

#define BITPRIM_RPC_VERSION "0.10.2"
#define BITPRIM_RPC_MAJOR_VERSION 0
#define BITPRIM_RPC_MINOR_VERSION 10
#define BITPRIM_RPC_PATCH_VERSION 2

// #define STR_HELPER(x) #x
// #define STR(x) STR_HELPER(x)
// #define BITPRIM_RPC_VERSION STR(BITPRIM_RPC_MAJOR_VERSION) "." STR(BITPRIM_RPC_MINOR_VERSION) "." STR(BITPRIM_RPC_PATCH_VERSION)
// #undef STR
// #undef STR_HELPER

#ifdef BITPRIM_BUILD_NUMBER
#define BITPRIM_RPC_VERSION BITPRIM_BUILD_NUMBER
#else
#define BITPRIM_RPC_VERSION "v0.0.0"
#endif

namespace bitprim { namespace rpc {
char const* version();
}} /*namespace bitprim::rpc*/
 
#endif /* BITPRIM_RPC_VERSION_HPP_ */
