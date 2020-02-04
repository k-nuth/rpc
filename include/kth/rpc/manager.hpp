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

#ifndef KTH_RPC_MANAGER_HPP_
#define KTH_RPC_MANAGER_HPP_

#include <knuth/rpc/http/rpc_server.hpp>
#include <knuth/rpc/zmq/zmq_helper.hpp>

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

}} //namespace knuth::rpc

#endif //KTH_RPC_MANAGER_HPP_

