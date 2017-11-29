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

#include <bitprim/rpc/zmq/zmq_helper.hpp>
namespace bitprim { namespace rpc {

zmq::zmq(uint32_t subscriber_port, libbitcoin::blockchain::block_chain & chain) :
        nSequence(0),
        chain_(chain){
    std::string str_port = "tcp://*:" + std::to_string (subscriber_port);
    context_ = zmq_init(1);
    if (context_) {
        publisher_ =  zmq_socket(context_,ZMQ_PUB);
        zmq_bind(publisher_, str_port.c_str());
        // If there is no sleep and a message is sent before 1 second, the publisher will not publish the message
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

zmq::~zmq(){
    close();
}

void zmq::close(){
    //TODO: should we do something with the subscribers?
    if (context_) {
        int linger = 0;
        zmq_setsockopt(publisher_, ZMQ_LINGER, &linger, sizeof(linger));
        zmq_close(publisher_);
        zmq_ctx_destroy(context_);
        context_ = 0;
    }
}

void zmq::start(){
    chain_.subscribe_blockchain([&](libbitcoin::code ec, size_t height,
                                    libbitcoin::block_const_ptr_list_const_ptr incoming,
                                    libbitcoin::block_const_ptr_list_const_ptr outgoing) {
        if (context_){
            return send_hash_block_handler(ec, height, incoming, outgoing);
        } else {
            // There is no context, the zmq was closed, so it unsubscribes
            return false;
        }
                                }
    );
    chain_.subscribe_transaction([&](libbitcoin::code ec, libbitcoin::transaction_const_ptr tx) {
        if (context_){
            return send_raw_transaction_handler(ec, tx);
        } else {
            // There is no context, the zmq was closed, so it unsubscribes
            return false;
        }
    });
}


static int zmq_send_multipart(void *sock, const void *data, size_t size, ...) {
    va_list args;
    va_start(args, size);

    while (true) {
        zmq_msg_t msg;

        int rc = zmq_msg_init_size(&msg, size);
        if (rc != 0) {
            return -1;
        }

        void *buf = zmq_msg_data(&msg);
        memcpy(buf, data, size);

        data = va_arg(args, const void *);

        rc = zmq_msg_send(&msg, sock, data ? ZMQ_SNDMORE : 0);
        if (rc == -1) {
            zmq_msg_close(&msg);
            return -1;
        }

        zmq_msg_close(&msg);

        if (!data) break;

        size = va_arg(args, size_t);
    }
    return 0;
}

bool zmq::send_message(const char *command, const void *data, size_t size) {
    /* send three parts, command & data & a LE 4byte sequence number */
    uint8_t msgseq[sizeof(uint32_t)];
    auto temp_msgseq = libbitcoin::to_little_endian(nSequence);
    std::copy(temp_msgseq.begin(), temp_msgseq.end(), msgseq);

    int rc = zmq_send_multipart(publisher_, command, strlen(command), data, size,
                                msgseq, (size_t) sizeof(uint32_t), (void *) 0);
    if (rc == -1) return false;

    /* increment memory only sequence number after sending */
    nSequence++;

    return true;
}

bool zmq::send_hash_block_handler(libbitcoin::code ec, size_t height,
                                         libbitcoin::block_const_ptr_list_const_ptr incoming,
                                         libbitcoin::block_const_ptr_list_const_ptr outgoing) {

    bool success = true;

    const char *MSG_HASHBLOCK = "hashblock";
    if (incoming) {
        for (const auto &block : *incoming) {
            for (auto const &tx : block->transactions()) {
                const char *MSG_RAWTX = "rawtx";
                auto const temp = tx.to_data(1);
                success = send_message(MSG_RAWTX, &(*temp.begin()), temp.size());
                if (!success) {
                    return success;
                }
            }
            auto temp = block->hash();
            char data[32];
            for (unsigned int i = 0; i < 32; i++)
                data[31 - i] = temp.begin()[i];
            success = send_message(MSG_HASHBLOCK, data, 32);
            if (!success) {
                break;
            }
        }
    }

    return success;
}

bool zmq::send_raw_transaction_handler(libbitcoin::code ec, libbitcoin::transaction_const_ptr incoming) {

    if (incoming) {
        const char *MSG_RAWTX = "rawtx";
        auto const temp = incoming->to_data(1);
        return send_message(MSG_RAWTX, &(*temp.begin()), temp.size());
    }

}

}}