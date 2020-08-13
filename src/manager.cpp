// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include <kth/rpc/manager.hpp>

namespace kth { namespace rpc {

manager::manager(bool use_testnet_rules
        , kth::node::full_node& node
        , uint32_t rpc_port
        , uint32_t subscriber_port
#ifdef KTH_WITH_KEOKEN
        , size_t keoken_genesis_height
#endif
        , std::unordered_set<std::string> const& rpc_allowed_ips
        , bool rpc_allow_all_ips)
   : stopped_(false)
   , zmq_(subscriber_port, node.chain_kth())
#ifdef KTH_WITH_KEOKEN
   , http_(use_testnet_rules, node, rpc_port, keoken_genesis_height, rpc_allowed_ips, rpc_allow_all_ips)
#else
   , http_(use_testnet_rules, node, rpc_port, rpc_allowed_ips, rpc_allow_all_ips)
#endif
{}

manager::~manager() {
    stop();
}

void manager::start() {
    stopped_ = false;
    zmq_.start();
    http_.start();
}

void manager::stop() {
    if ( ! stopped_) {
        zmq_.close();
        http_.stop();
    }
    stopped_ = true;
}

bool manager::is_stopped() const {
    return stopped_;
}

}} //namespace kth::rpc