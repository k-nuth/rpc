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

#ifndef BITPRIM_MANAGER_HPP
#define BITPRIM_MANAGER_HPP

#include <bitprim/rpc/http/rpc_server.hpp>
#include <bitprim/rpc/zmq/zmq_helper.hpp>

namespace bitprim { namespace rpc {
class manager{
public:
    manager(bool use_testnet_rules, libbitcoin::blockchain::block_chain & chain, uint32_t rpc_port, uint32_t subscriber_port);
    ~manager();
    void start();
    void stop();
    bool is_stopped();

private:
    bool stopped;
    zmq zmq_;
    rpc_server http_;

};
}
}
#endif //BITPRIM_MANAGER_HPP
