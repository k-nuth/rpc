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

#include <bitprim/rpc/rpc_server.hpp>

#include <iostream>

namespace bitprim { namespace rpc {

rpc_server::rpc_server(bool use_testnet_rules, libbitcoin::blockchain::block_chain & chain, uint32_t rpc_port, uint32_t subscriber_port)
    : use_testnet_rules_(use_testnet_rules)
    , stopped_(true)
    , chain_(chain)
    , nSequence(0)
{
    std::string str_port = "tcp://*:" + std::to_string (subscriber_port);

    context_ = zmq_init(1);
    if (context_) {
        publisher_ =  zmq_socket(context_,ZMQ_PUB);
        // TODO: change the port
        zmq_bind(publisher_, str_port.c_str());
    }
    server_.config.port = rpc_port;
    configure_server();
}

void rpc_server::configure_server() {

    server_.resource["^/json$"]["POST"] = [this](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        //TODO: validate json parameters
        try {
            auto json_str = request->content.string();
            if (json_str.size() > 0 && json_str.back() == '\n') {
                json_str.pop_back();
            }
            nlohmann::json json_object = nlohmann::json::parse(json_str);

            auto result = bitprim::process_data(json_object, use_testnet_rules_, chain_);
            result = result + "\u000a";

            *response << "HTTP/1.1 200 OK\r\n"
                        << "Content-Type: application/json\r\n"
                        << "Content-Length: " << result.length() << "\r\n\r\n"
                        << result;

        } catch(std::exception const& e) {
            *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
        }
    };

    server_.default_resource["POST"] = [this](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        //TODO: validate if request is application/json
        try {
//            nlohmann::json json_object = nlohmann::json::parse(request->content.string());

            auto json_str = request->content.string();

            if (json_str.size() > 0 && json_str.back() == '\n') {
                json_str.pop_back();
            }

            nlohmann::json json_object = nlohmann::json::parse(json_str);

            auto result = bitprim::process_data(json_object, use_testnet_rules_, chain_);
            result = result + "\u000a";

//            TODO: add date to response
//            << "Date: Wed, 01 Feb 2017 15:03:36 GMT\r\n"
            *response << "HTTP/1.1 200 OK\r\n"
                        << "Content-Type: application/json\r\n"
                        << "Content-Length: " << result.length() << "\r\n\r\n"
                        << result;

        } catch(std::exception const& e) {
            *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
        }
    };

    server_.default_resource["GET"] = [](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        //TODO: check error description
        std::string error = "This server only accepts json requests";
        *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << error.length() << "\r\n\r\n" << error;
    };
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

bool rpc_server::SendMessage(const char *command, const void *data, size_t size) {
    /* send three parts, command & data & a LE 4byte sequence number */
    uint8_t msgseq[sizeof(uint32_t)];
    auto temp_msgseq = libbitcoin::to_little_endian(nSequence);
    std::copy(temp_msgseq.begin(),temp_msgseq.end(),msgseq);

    int rc = zmq_send_multipart(publisher_, command, strlen(command), data, size,
                                msgseq, (size_t)sizeof(uint32_t), (void *)0);
    if (rc == -1) return false;

    /* increment memory only sequence number after sending */
    nSequence++;

    return true;
}

bool rpc_server::send_hash_block_handler(libbitcoin::code ec, size_t height,
                                            libbitcoin::block_const_ptr_list_const_ptr incoming,
                                            libbitcoin::block_const_ptr_list_const_ptr outgoing){

    bool success = true;

    const char *MSG_HASHBLOCK = "hashblock";
    if (incoming){
        for (const auto& block : *incoming){
            for (auto const & tx : block->transactions()){
                const char *MSG_RAWTX = "rawtx";
                auto const temp = tx.to_data(1);
                success = SendMessage(MSG_RAWTX, &(*temp.begin()), temp.size());
                if (!success) {
                    return success;
                }
            }
            auto temp = block->hash();
            char data[32];
            for (unsigned int i = 0; i < 32; i++)
                data[31 - i] = temp.begin()[i];
            success = SendMessage(MSG_HASHBLOCK, data, 32);
            if (!success) {
                break;
            }
        }
    }

    return success;
}

bool rpc_server::send_raw_transaction_handler(libbitcoin::code ec,libbitcoin::transaction_const_ptr incoming) {

    if (incoming){
        const char *MSG_RAWTX = "rawtx";
        auto const temp = incoming->to_data(1);
        return SendMessage(MSG_RAWTX, &(*temp.begin()), temp.size());
    }

}

bool rpc_server::start() {
    stopped_ = false;
    bool subscribe = true;
    if (subscribe) {
        chain_.subscribe_blockchain([&](libbitcoin::code ec, size_t height,
                                        libbitcoin::block_const_ptr_list_const_ptr incoming,
                                        libbitcoin::block_const_ptr_list_const_ptr outgoing) {
                                        return send_hash_block_handler(ec, height, incoming, outgoing);
                                    }
        );
        chain_.subscribe_transaction([&](libbitcoin::code ec, libbitcoin::transaction_const_ptr tx) {
            return send_raw_transaction_handler(ec, tx);
        });
    }

    server_.start();
    return true;
}

bool rpc_server::stop() {
    stopped_ = true;
    //stop-zmq
    if (context_) {
        int linger = 0;
        zmq_setsockopt(publisher_, ZMQ_LINGER, &linger, sizeof(linger));
        zmq_close(publisher_);
        zmq_ctx_destroy(context_);
        context_ = 0;
    }

    server_.stop();
    return true;
}

bool rpc_server::stopped() const {
    return stopped_;
}

}} // namespace bitprim::rpc
