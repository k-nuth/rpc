// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_RPC_MANAGER_HPP_
#define KTH_RPC_MANAGER_HPP_

#include <kth/rpc/http/rpc_server.hpp>
#include <kth/rpc/zmq/zmq_helper.hpp>

namespace kth { namespace rpc {

class manager {
public:
    manager(bool use_testnet_rules
            , kth::node::full_node& node
            , uint32_t rpc_port
            , uint32_t subscriber_port
#ifdef KTH_WITH_KEOKEN
            , size_t keoken_genesis_height
#endif
            , std::unordered_set<std::string> const& rpc_allowed_ips
            , bool rpc_allow_all_ips);
   ~manager();

   void start();
   void stop();
   bool is_stopped() const;

private:
   bool stopped_;
   zmq zmq_;
   rpc_server http_;
};

}} //namespace kth::rpc

#endif //KTH_RPC_MANAGER_HPP_

