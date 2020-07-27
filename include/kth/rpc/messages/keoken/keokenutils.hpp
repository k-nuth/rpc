// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_MESSAGES_KEOKENUTILS_HPP_
#define KTH_RPC_MESSAGES_KEOKENUTILS_HPP_

#include <kth/blockchain/interface/block_chain.hpp>
include <kth/keoken/message/create_asset.hpp>
include <kth/keoken/message/send_tokens.hpp>
include <kth/keoken/transaction_extractor.hpp>
include <kth/keoken/transaction_processors/commons.hpp>
#include <kth/rpc/json/json.hpp>

namespace kth {

template <typename Blockchain>
nlohmann::json decode_keoken( Blockchain const& chain, kth::transaction_const_ptr tx_ptr, bool testnet = false) {

    nlohmann::json container;

    auto keoken_data = kth::keoken::first_keoken_output(*tx_ptr);
    if ( ! keoken_data.empty()) {
        kth::data_source ds(keoken_data);
        kth::istream_reader source(ds);

        auto version = source.read_2_bytes_big_endian();
        if ( ! source) return {};
        auto type = source.read_2_bytes_big_endian();
        if ( ! source) return {};
        container["version"] = version;
        container["type"] = type;
        container["hash"] = kth::encode_hash((*tx_ptr).hash());
        switch (type) {
        case kth::keoken::message::create_asset::type: {
            auto create = domain::create<kth::keoken::message::create_asset>(source);
            container["asset"] = create.name();
            container["amount"] =  create.amount();
            container["owner"] = (kth::keoken::get_first_input_addr(chain, *tx_ptr, testnet)).encoded();
            break;
        }
        case kth::keoken::message::send_tokens::type: {
            auto message = domain::create<kth::keoken::message::send_tokens>(source);
            container["asset"] = message.asset_id();
            container["amount"] = message.amount();
            auto addresses = kth::keoken::get_send_tokens_addrs(chain, *tx_ptr, testnet);
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

static
kth::domain::wallet::payment_address to_mainnet_addr (kth::domain::wallet::payment_address const& input_addr) {
    //Generate an address


    uint8_t prefix;
    if (input_addr.version() == kth::domain::wallet::payment_address::testnet_p2sh) {
        //It's p2sh testnet
        prefix = kth::domain::wallet::payment_address::mainnet_p2sh;
    } else if (input_addr.version() == kth::domain::wallet::payment_address::testnet_p2kh) {
        //It's p2kh testnet
        prefix = kth::domain::wallet::payment_address::mainnet_p2kh;
    } else {
        // It's not testnet
        return input_addr.encoded();
    }

    // The wallet is testnet
    kth::domain::wallet::payment_address new_wallet(input_addr.hash(), prefix);
    return new_wallet;
}

static
kth::domain::wallet::payment_address to_testnet_addr (kth::domain::wallet::payment_address const& input_addr) {
    uint8_t testnet_prefix;
    if (input_addr.version() == kth::domain::wallet::payment_address::mainnet_p2sh) {
        //It's p2sh mainnet
        testnet_prefix = kth::domain::wallet::payment_address::testnet_p2sh;
    } else if (input_addr.version() == kth::domain::wallet::payment_address::mainnet_p2kh) {
        //It's p2kh mainnet
        testnet_prefix = kth::domain::wallet::payment_address::testnet_p2kh;
    } else {
        // It's not mainnet
        return input_addr.encoded();
    }

    // The wallet is mainnet
    kth::domain::wallet::payment_address testnet_wallet(input_addr.hash(), testnet_prefix);
    return testnet_wallet;
}

static
kth::domain::wallet::payment_address str_to_mainnet_addr (std::string const& input) {
    //Generate an address
    auto input_addr = kth::domain::wallet::payment_address(input);
    return to_mainnet_addr(input_addr);
}

static
kth::domain::wallet::payment_address str_to_testnet_addr (std::string const& input) {
    //Generate an address
    auto input_addr = kth::domain::wallet::payment_address(input);
    return to_testnet_addr(input_addr);
}


static
kth::domain::wallet::payment_address str_to_network_wallet(bool testnet, std::string const& input) {
    if ( ! testnet)
        return str_to_mainnet_addr(input);
    else
        return str_to_testnet_addr(input);
}
static
kth::domain::wallet::payment_address to_network_wallet(bool testnet, kth::domain::wallet::payment_address const& input_addr) {
    if ( ! testnet)
        return to_mainnet_addr(input_addr);
    else
        return to_testnet_addr(input_addr);
}

}

#endif
