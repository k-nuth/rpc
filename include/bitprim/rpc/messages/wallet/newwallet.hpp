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

#ifndef BITPRIM_RPC_MESSAGES_NEWWALLET_HPP_
#define BITPRIM_RPC_MESSAGES_NEWWALLET_HPP_

#include <bitprim/rpc/json/json.hpp>
#include <bitcoin/blockchain/interface/block_chain.hpp>

#include <bitprim/rpc/messages/error_codes.hpp>
#include <bitprim/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

// #include <bitcoin/bitcoin/constants.hpp>
// #include <bitcoin/bitcoin/utility/pseudo_random.hpp>
#include <bitcoin/bitcoin.hpp>

namespace bitprim {


// TODO:move this function to bitprim-core
inline
libbitcoin::data_chunk create_new_seed(size_t bit_length = libbitcoin::minimum_seed_bits) {
  size_t fill_seed_size = bit_length / libbitcoin::byte_bits;
  libbitcoin::data_chunk seed(fill_seed_size);
  libbitcoin::pseudo_random_fill(seed);
  return seed;
};

// TODO:move this function to bitprim-core
inline
libbitcoin::ec_secret generate_priv_key(libbitcoin::data_chunk const &seed) {
// This check is needed on bitprim-core, here we always pass a valid seed.
//   if (seed.size() < libbitcoin::minimum_seed_size) {
//     // Short seed
//     return {}};
//   }
  const libbitcoin::wallet::hd_private key(seed);
  libbitcoin::ec_secret secret(key.secret());
// This check is needed on bitprim-core, here this validation in called in seed_to_wallet.
//   if (secret == libbitcoin::null_hash) {
//     // New invalid key
//     return {};
//   }
  return secret;
}

// TODO:move this function to bitprim-core
inline
libbitcoin::wallet::ec_public priv_key_to_public(libbitcoin::ec_secret const &priv_key, bool compress) {
  libbitcoin::ec_compressed point;
  libbitcoin::secret_to_public(point, priv_key);
  return libbitcoin::wallet::ec_public(point, compress);
}

// TODO:move this function to bitprim-core
inline
libbitcoin::wallet::payment_address pub_key_to_addr(libbitcoin::wallet::ec_public const &pub_key, bool mainnet) {
  uint8_t version;
  if (mainnet) {
    version = libbitcoin::wallet::payment_address::mainnet_p2kh;
  } else {
    version = libbitcoin::wallet::payment_address::testnet_p2kh;
  }
  return libbitcoin::wallet::payment_address(pub_key, version);
}

inline
std::string seed_to_wallet(libbitcoin::data_chunk const& seed, bool compressed, bool mainnet) {
  auto priv_key = generate_priv_key(seed);
  // ec public has the operator bool
  if (priv_key == libbitcoin::null_hash) return "";
  auto pub_key = priv_key_to_public(priv_key, compressed).encoded();
  auto new_wallet = pub_key_to_addr(pub_key, mainnet);
  return new_wallet.encoded();
}



// Start RPC functions
inline
bool json_in_newwallet(nlohmann::json const& json_object, 
                              bool & compressed, bool & mainnet){
    auto const & size = json_object["params"].size();
    if (size == 0)
        return false;
    try {
      compressed = json_object["params"]["compressed"];
      mainnet = json_object["params"]["mainnet"];
    }
    catch (const std::exception & e) {
        return false;
    }
    return true;

}


template <typename Node>
bool newwallet(nlohmann::json& json_object, bool compressed, bool mainnet, bool use_testnet_rules, Node& node)
{
    auto const seed = create_new_seed();
    json_object["seed"] = libbitcoin::encode_base16(seed);
    json_object["addr"] = seed_to_wallet(seed, compressed, mainnet);
    return true;
}

template <typename Node>
nlohmann::json process_newwallet(nlohmann::json const& json_in, Node& node, bool use_testnet_rules)
{
    nlohmann::json container, result;
    container["id"] = json_in["id"];
    bool compressed;
    bool mainnet;

    int error = 0;
    std::string error_code;

    if (!json_in_newwallet(json_in, compressed, mainnet)) //if false return error
    {
        container["result"];
        container["error"]["code"] = bitprim::RPC_PARSE_ERROR;
        container["error"]["message"] = "Parse error.";
        return container;
    }
    if (newwallet(result, compressed, mainnet, use_testnet_rules, node))
    {
        container["result"] = result;
        container["error"];
    }
    else {
        container["error"]["code"] = error;
        container["error"]["message"] = error_code;
    }

    return container;
}

} //namespace bitprim

#endif //BITPRIM_RPC_MESSAGES_NEWWALLET_HPP_
