/**
 * Copyright (c) 2017-2018 Bitprim Inc.
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

#ifdef BITPRIM_PROJECT_VERSION
#define BITPRIM_RPC_VERSION BITPRIM_PROJECT_VERSION
#else
#define BITPRIM_RPC_VERSION "0.0.0"
#endif

namespace bitprim { namespace rpc {
char const* version();
}} /*namespace bitprim::rpc*/
 
#endif /* BITPRIM_RPC_VERSION_HPP_ */
