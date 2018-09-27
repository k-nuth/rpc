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

#include <bitprim/rpc/manager.hpp>

namespace bitprim { namespace rpc {

manager::manager(bool use_testnet_rules
        , libbitcoin::node::full_node& node
        , uint32_t rpc_port
        , uint32_t subscriber_port
#ifdef WITH_KEOKEN
        , size_t keoken_genesis_height
#endif
        , std::unordered_set<std::string> const& rpc_allowed_ips
        , bool rpc_allow_all_ips)
   : stopped_(false)
   , zmq_(subscriber_port, node.chain_bitprim())
#ifdef WITH_KEOKEN
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
    if (!stopped_) {
        zmq_.close();
        http_.stop();
    }
    stopped_ = true;
}

bool manager::is_stopped() const {
    return stopped_;
}

}} //namespace bitprim::rpc