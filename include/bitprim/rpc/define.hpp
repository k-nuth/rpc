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

#ifndef BITPRIM_RPC_DEFINE_HPP_
#define BITPRIM_RPC_DEFINE_HPP_

#include <bitcoin/bitcoin.hpp>

#if defined BCR_STATIC
#define BCR_API
#define BCR_INTERNAL
#elif defined BCR_DLL
#define BCR_API      BC_HELPER_DLL_EXPORT
#define BCR_INTERNAL BC_HELPER_DLL_LOCAL
#else
#define BCR_API      BC_HELPER_DLL_IMPORT
#define BCR_INTERNAL BC_HELPER_DLL_LOCAL
#endif

// Log name.
#define LOG_RPC "rpc"

#endif /* BITPRIM_RPC_DEFINE_HPP_ */ 
