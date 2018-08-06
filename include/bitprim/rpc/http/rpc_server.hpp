/**
 * Copyright (c) 2017-2018 Bitprim Inc.
 *
 * This file is part of bitprim-node.
 *
 * bitprim-node is free software: you can redistribute it and/or
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

#ifndef BITPRIM_RPC_SERVER_HPP_
#define	BITPRIM_RPC_SERVER_HPP_


#include <string>
#include <unordered_map>
#include <unordered_set>

#include <zmq.h>

#include <bitcoin/blockchain/interface/block_chain.hpp>
#include <bitcoin/network/p2p.hpp>
#include <bitcoin/node/full_node.hpp>

#include <bitprim/rpc/http/server_http.hpp>
#include <bitprim/rpc/json/json.hpp>
#include <bitprim/rpc/messages.hpp>

#ifdef WITH_KEOKEN
#include <bitprim/keoken/manager.hpp>
#include <bitprim/keoken/state.hpp>
#endif


namespace bitprim { namespace rpc {

class rpc_server {
    using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
public:
#ifdef WITH_KEOKEN
    using keoken_manager_t = bitprim::keoken::manager<bitprim::keoken::state>;
#endif

    rpc_server(bool use_testnet_rules
            , libbitcoin::node::full_node& node
            , uint32_t rpc_port
#ifdef WITH_KEOKEN
            , size_t keoken_genesis_height
#endif
            , std::unordered_set<std::string> const& rpc_allowed_ips);

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
    //int port_;
    HttpServer server_;

    // If the subscribe methods are removed from here the chain_ can be const
    libbitcoin::node::full_node& node_;

#ifdef WITH_KEOKEN
    keoken_manager_t keoken_manager_;
#endif

    signature_map<libbitcoin::blockchain::block_chain> signature_map_;
    signature_map<libbitcoin::blockchain::block_chain> signature_map_no_params_;
    std::unordered_set<std::string> rpc_allowed_ips_;
};

}} // namespace bitprim::rpc

#endif /*BITPRIM_RPC_SERVER_HPP_*/
