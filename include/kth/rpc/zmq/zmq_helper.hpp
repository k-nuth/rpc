// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_ZMQ_HELPER_HPP
#define KTH_ZMQ_HELPER_HPP

#include <chrono>
#include <thread>

#include <kth/blockchain.hpp>

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
    kth::blockchain::block_chain & chain_;
};

}}

#endif //KTH_ZMQ_HELPER_HPP
