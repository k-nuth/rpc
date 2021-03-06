// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_RPC_SERVER_HPP_
#define	KTH_RPC_SERVER_HPP_

#include <kth/blockchain/interface/block_chain.hpp>
#include <kth/network/p2p.hpp>
#include <kth/node/full_node.hpp>

#include <kth/rpc/http/server_http.hpp>
#include <kth/rpc/json/json.hpp>
#include <kth/rpc/messages.hpp>

#ifdef KTH_WITH_KEOKEN
include <kth/keoken/manager.hpp>
include <kth/keoken/memory_state.hpp>
#endif

//Note: do not change the order of the following headers (compilation error in Windows)
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <zmq.h>


namespace kth { namespace rpc {

class rpc_server {
    using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
public:
#ifdef KTH_WITH_KEOKEN
    using keoken_manager_t = kth::keoken::manager<kth::keoken::memory_state>;
#endif

    rpc_server(bool use_testnet_rules
            , kth::node::full_node& node
            , uint32_t rpc_port
#ifdef KTH_WITH_KEOKEN
            , size_t keoken_genesis_height
#endif
            , std::unordered_set<std::string> const& rpc_allowed_ips
            , bool rpc_allow_all_ips);

    //non-copyable
    rpc_server(rpc_server const&) = delete;
    rpc_server& operator=(rpc_server const&) = delete;

    bool start();
    bool stop();
    bool stopped() const;

private:        
    void configure_server();

    bool use_testnet_rules_;
    bool stopped_;
    bool rpc_allow_all_ips_;
    //int port_;
    HttpServer server_;

    // If the subscribe methods are removed from here the chain_ can be const
    kth::node::full_node& node_;

#ifdef KTH_WITH_KEOKEN
    keoken_manager_t keoken_manager_;
#endif

    signature_map<kth::blockchain::block_chain> signature_map_;
    signature_map<kth::blockchain::block_chain> signature_map_no_params_;
    std::unordered_set<std::string> rpc_allowed_ips_;

};

}} // namespace kth::rpc

#endif /*KTH_RPC_SERVER_HPP_*/
