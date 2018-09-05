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

#ifndef BITPRIM_RPC_MESSAGES_KEOKENUTILS_HPP_
#define BITPRIM_RPC_MESSAGES_KEOKENUTILS_HPP_

#include <bitcoin/blockchain/interface/block_chain.hpp>
#include <bitprim/keoken/message/create_asset.hpp>
#include <bitprim/keoken/message/send_tokens.hpp>
#include <bitprim/keoken/transaction_extractor.hpp>
#include <bitprim/keoken/transaction_processors/commons.hpp>
#include <bitprim/rpc/json/json.hpp>

namespace bitprim {

template <typename Blockchain>
nlohmann::json decode_keoken( Blockchain const& chain, libbitcoin::transaction_const_ptr tx_ptr) {

    nlohmann::json container;

    auto keoken_data = bitprim::keoken::first_keoken_output(*tx_ptr);
    if ( ! keoken_data.empty()) {
        libbitcoin::data_source ds(keoken_data);
        libbitcoin::istream_reader source(ds);

        auto version = source.read_2_bytes_big_endian();
        if ( ! source) return {};
        auto type = source.read_2_bytes_big_endian();
        if ( ! source) return {};
        container["version"] = version;
        container["type"] = type;
        switch (type) {
        case 0: {
            auto create = bitprim::keoken::message::create_asset::factory_from_data(source);
            container["asset"] = create.name();
            container["amount"] =  create.amount();
            container["owner"] = (bitprim::keoken::get_first_input_addr(chain, *tx_ptr)).encoded();
            break;
        }
        case 1: {
            auto message = bitprim::keoken::message::send_tokens::factory_from_data(source);
            container["asset"] = message.asset_id();
            container["amount"] = message.amount();
            auto addresses = bitprim::keoken::get_send_tokens_addrs(chain, *tx_ptr);
            container["sender"] = (addresses.first).encoded();
            container["receiver"] = (addresses.second).encoded();
            break;
        }
        default:
            break;
        }
    }
    return container;
}

}

#endif
