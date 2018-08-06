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

#include <bitprim/rpc/http/rpc_server.hpp>

#include <iostream>

namespace bitprim { namespace rpc {

rpc_server::rpc_server(bool use_testnet_rules
        , libbitcoin::node::full_node& node
        , uint32_t rpc_port
#ifdef WITH_KEOKEN
        , size_t keoken_genesis_height
#endif
        , std::unordered_set<std::string> const& rpc_allowed_ips)
    : use_testnet_rules_(use_testnet_rules)
    , stopped_(true)
    , node_(node)
#ifdef WITH_KEOKEN
    , keoken_manager_(node.chain_bitprim(), keoken_genesis_height)
#endif
    , rpc_allowed_ips_(rpc_allowed_ips)
    , signature_map_(load_signature_map<libbitcoin::blockchain::block_chain>())
    , signature_map_no_params_(load_signature_map_no_params<libbitcoin::blockchain::block_chain>())
{
    server_.config.port = rpc_port;
    configure_server();
}

void rpc_server::configure_server() {

    server_.resource["^/json$"]["POST"] = [this](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        //TODO: validate json parameters
        if (rpc_allowed_ips_.find(request->remote_endpoint_address) != rpc_allowed_ips_.end()){
            try {
                auto json_str = request->content.string();
                if (json_str.size() > 0 && json_str.back() == '\n') {
                    json_str.pop_back();
                }
                nlohmann::json json_object = nlohmann::json::parse(json_str);

#ifdef WITH_KEOKEN
                auto result = bitprim::process_data(json_object, use_testnet_rules_, node_, keoken_manager_, signature_map_, signature_map_no_params_);
#else
                auto result = bitprim::process_data(json_object, use_testnet_rules_, node_, signature_map_, signature_map_no_params_);
#endif
                result = result + "\u000a";

                *response << "HTTP/1.1 200 OK\r\n"
                            << "Content-Type: application/json\r\n"
                            << "Content-Length: " << result.length() << "\r\n\r\n"
                            << result;

            } catch(std::exception const& e) {
                *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << strlen(e.what()) << "\r\n\r\n" << e.what();
            }
        }else {
            std::string e = "HTTP_FORBIDDEN";
            *response << "HTTP/1.1 403 Forbidden\r\nContent-Length: " << e.length() << "\r\n\r\n" << e;
        }
    };

    server_.default_resource["POST"] = [this](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        //TODO: validate if request is application/json

        if (rpc_allowed_ips_.find(request->remote_endpoint_address) != rpc_allowed_ips_.end()){
            try {
    //            nlohmann::json json_object = nlohmann::json::parse(request->content.string());

                auto json_str = request->content.string();

                if (json_str.size() > 0 && json_str.back() == '\n') {
                    json_str.pop_back();
                }

                nlohmann::json json_object = nlohmann::json::parse(json_str);

#ifdef WITH_KEOKEN
                auto result = bitprim::process_data(json_object, use_testnet_rules_, node_, keoken_manager_, signature_map_, signature_map_no_params_);
#else
                auto result = bitprim::process_data(json_object, use_testnet_rules_, node_, signature_map_, signature_map_no_params_);
#endif

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
        }else {
            std::string e = "HTTP_FORBIDDEN";
            *response << "HTTP/1.1 403 Forbidden\r\nContent-Length: " << e.length() << "\r\n\r\n" << e;
        }
    };

    server_.default_resource["GET"] = [](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
        //TODO: check error description
        std::string error = "This server only accepts json requests";
        *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: " << error.length() << "\r\n\r\n" << error;
    };
}

bool rpc_server::start() {
    stopped_ = false;
    server_.start();
    return true;
}

bool rpc_server::stop() {
    stopped_ = true;
    server_.stop();
    return true;
}

bool rpc_server::stopped() const {
    return stopped_;
}

}} // namespace bitprim::rpc
