/**
 * Copyright (c) 2017 Bitprim developers (see AUTHORS)
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

#include <bitprim/rpc/http/server_http.hpp>

#include <bitprim/rpc/json/json.hpp>
#include <bitprim/rpc/messages.hpp>         
#include <bitcoin/blockchain/interface/block_chain.hpp>

#include <zmq.h>

namespace bitprim { namespace rpc {

class rpc_server {
    using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
public:
    rpc_server(bool use_testnet_rules, libbitcoin::blockchain::block_chain & chain, uint32_t rpc_port);
    //non-copyable
    rpc_server(rpc_server const&) = delete;
    rpc_server& operator=(rpc_server const&) = delete;

    bool start();
    bool stop();
    bool stopped() const;

private:        
    void configure_server();
    const bool use_testnet_rules_;
    bool stopped_;      
    //int port_;
    HttpServer server_;
    // If the subscribe methods are removed from here
    // the chain_ can be const
    libbitcoin::blockchain::block_chain & chain_;
};

}} // namespace bitprim::rpc

#endif /*BITPRIM_RPC_SERVER_HPP_*/
