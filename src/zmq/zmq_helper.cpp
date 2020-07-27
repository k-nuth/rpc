// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#include <kth/rpc/zmq/zmq_helper.hpp>

namespace kth { namespace rpc {

zmq::zmq(uint32_t subscriber_port, kth::blockchain::block_chain & chain) :
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
    if (context_) {
        int linger = 0;
        zmq_setsockopt(publisher_, ZMQ_LINGER, &linger, sizeof(linger));
        zmq_close(publisher_);
        zmq_ctx_destroy(context_);
        context_ = 0;
    }
}

void zmq::start(){
    // Only send messages when the chain is not stale
    if (!chain_.is_stale()){
        start_sending_messages();
    } else {
        // Check until the chain is no longer stale
        chain_.subscribe_blockchain([&](kth::code ec, size_t height,
                                        kth::block_const_ptr_list_const_ptr incoming,
                                        kth::block_const_ptr_list_const_ptr outgoing) {
            if (chain_.is_stale()){
                return true;
            } else {
                start_sending_messages();
                return false;
            }
        });
    }
}

void zmq::start_sending_messages(){
    chain_.subscribe_blockchain([&](kth::code ec, size_t height,
                                    kth::block_const_ptr_list_const_ptr incoming,
                                    kth::block_const_ptr_list_const_ptr outgoing) {
                                    if (context_){
                                        return send_hash_block_handler(ec, height, incoming, outgoing);
                                    } else {
                                        // There is no context, the zmq was closed, so it unsubscribes
                                        return false;
                                    }
                                }
    );
    chain_.subscribe_transaction([&](kth::code ec, kth::transaction_const_ptr tx) {
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
    auto temp_msgseq = kth::to_little_endian(nSequence);
    std::copy(temp_msgseq.begin(), temp_msgseq.end(), msgseq);

    int rc = zmq_send_multipart(publisher_, command, strlen(command), data, size,
                                msgseq, (size_t) sizeof(uint32_t), (void *) 0);
    if (rc == -1) return false;

    /* increment memory only sequence number after sending */
    nSequence++;

    return true;
}

bool zmq::send_hash_block_handler(kth::code ec, size_t height,
                                         kth::block_const_ptr_list_const_ptr incoming,
                                         kth::block_const_ptr_list_const_ptr outgoing) {

    bool success = true;

    const char *MSG_HASHBLOCK = "hashblock";
    if (incoming) {
        for (auto const &block : *incoming) {
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

bool zmq::send_raw_transaction_handler(kth::code ec, kth::transaction_const_ptr incoming) {

    if (incoming) {
        const char *MSG_RAWTX = "rawtx";
        auto const temp = incoming->to_data(1, false);
        return send_message(MSG_RAWTX, &(*temp.begin()), temp.size());
    }

    return true;
}

}}
