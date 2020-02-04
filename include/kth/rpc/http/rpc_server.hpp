/**
 * Copyright (c) 2016-2020 Knuth Project developers.
 *
 * This file is part of kth-node.
 *
 * kth-node is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the     
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KTH_RPC_SERVER_HPP_
#define	KTH_RPC_SERVER_HPP_

#include <bitcoin/blockchain/interface/block_chain.hpp>
#include <bitcoin/network/p2p.hpp>
#include <bitcoin/node/full_node.hpp>

#include <knuth/rpc/http/server_http.hpp>
#include <knuth/rpc/json/json.hpp>
#include <knuth/rpc/messages.hpp>

#ifdef KTH_WITH_KEOKEN
#include <knuth/keoken/manager.hpp>
#include <knuth/keoken/memory_state.hpp>
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
    using keoken_manager_t = knuth::keoken::manager<knuth::keoken::memory_state>;
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

}} // namespace knuth::rpc

#endif /*KTH_RPC_SERVER_HPP_*/
