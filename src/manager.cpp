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
    if (!stopped_) {
        zmq_.close();
        http_.stop();
    }
    stopped_ = true;
}

bool manager::is_stopped() const {
    return stopped_;
}

}} //namespace kth::rpc