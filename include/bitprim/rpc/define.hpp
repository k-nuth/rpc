// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_DEFINE_HPP_
#define KTH_RPC_DEFINE_HPP_

#include <kth/domain.hpp>

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

#endif /* KTH_RPC_DEFINE_HPP_ */ 
