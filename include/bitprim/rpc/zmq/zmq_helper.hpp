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

#ifndef KTH_ZMQ_HELPER_HPP
#define KTH_ZMQ_HELPER_HPP

#include <chrono>
#include <thread>

#include <bitcoin/blockchain.hpp>

#include <zmq.h>

namespace kth { namespace rpc {

class zmq {
public:
    zmq(uint32_t subscriber_port, kth::blockchain::block_chain & chain);
    //non-copyable
    zmq(zmq const&) = delete;
    zmq& operator=(zmq const&) = delete;
    ~zmq();
    void close();
    void start();
    void start_sending_messages();

public:
    bool send_message(const char *command, const void *data, size_t size);

    //Publisher methods
    bool send_hash_block_handler(kth::code ec, size_t height, kth::block_const_ptr_list_const_ptr incoming,
                                 kth::block_const_ptr_list_const_ptr outgoing);
    bool send_raw_transaction_handler(kth::code ec, kth::transaction_const_ptr incoming);

    bool send_random_data () {
        char const* MSG_RAWTX = "rawtx";
        std::string temp = "ESTO ES UN MENSAJE DE PRUEBAS";
        return send_message(MSG_RAWTX, &(*temp.data()), temp.length());
    };

private:
    // ZMQ
    void *context_;
    void *publisher_;
    uint32_t nSequence;
    // BITPRIM
    kth::blockchain::block_chain & chain_;
};

}}

#endif //KTH_ZMQ_HELPER_HPP
