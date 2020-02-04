// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_VERSION_HPP_
#define KTH_RPC_VERSION_HPP_

#ifdef KTH_PROJECT_VERSION
#define KTH_RPC_VERSION KTH_PROJECT_VERSION
#else
#define KTH_RPC_VERSION "0.0.0"
#endif

namespace kth { namespace rpc {
char const* version();
}} /*namespace knuth::rpc*/
 
#endif /* KTH_RPC_VERSION_HPP_ */
