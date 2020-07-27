// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_MESSAGES_NEWWALLET_HPP_
#define KTH_RPC_MESSAGES_NEWWALLET_HPP_

#include <kth/rpc/json/json.hpp>
#include <kth/blockchain/interface/block_chain.hpp>

#include <kth/rpc/messages/error_codes.hpp>
#include <kth/rpc/messages/utils.hpp>
#include <boost/thread/latch.hpp>

// #include <kth/domain/constants.hpp>
// #include <kth/domain/utility/pseudo_random.hpp>
#include <kth/domain.hpp>

namespace kth {


// TODO:move this function to kth-domain
inline
kth::data_chunk create_new_seed(size_t bit_length = kth::minimum_seed_bits) {
  size_t fill_seed_size = bit_length / kth::byte_bits;
  kth::data_chunk seed(fill_seed_size);
  kth::pseudo_random_fill(seed);
  return seed;
};

// TODO:move this function to kth-domain
inline
kth::ec_secret generate_priv_key(kth::data_chunk const &seed) {
// This check is needed on kth-domain, here we always pass a valid seed.
//   if (seed.size() < kth::minimum_seed_size) {
//     // Short seed
//     return {}};
//   }
  const kth::infrastructure::wallet::hd_private key(seed);
  kth::ec_secret secret(key.secret());
// This check is needed on kth-domain, here this validation in called in seed_to_wallet.
//   if (secret == kth::null_hash) {
//     // New invalid key
//     return {};
//   }
  return secret;
}

// TODO:move this function to kth-domain
inline
kth::domain::wallet::ec_public priv_key_to_public(kth::ec_secret const &priv_key, bool compress) {
  kth::ec_compressed point;
  kth::secret_to_public(point, priv_key);
  return kth::domain::wallet::ec_public(point, compress);
}

// TODO:move this function to kth-domain
inline
kth::domain::wallet::payment_address pub_key_to_addr(kth::domain::wallet::ec_public const &pub_key, bool mainnet) {
  uint8_t version;
  if (mainnet) {
    version = kth::domain::wallet::payment_address::mainnet_p2kh;
  } else {
    version = kth::domain::wallet::payment_address::testnet_p2kh;
  }
  return kth::domain::wallet::payment_address(pub_key, version);
}

inline
std::string seed_to_wallet(kth::data_chunk const& seed, bool compressed, bool mainnet) {
  auto priv_key = generate_priv_key(seed);
  // ec public has the operator bool
  if (priv_key == kth::null_hash) return "";
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
    json_object["seed"] = kth::encode_base16(seed);
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

    if ( ! json_in_newwallet(json_in, compressed, mainnet)) //if false return error
    {
        container["result"];
        container["error"]["code"] = kth::RPC_PARSE_ERROR;
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

} //namespace kth

#endif //KTH_RPC_MESSAGES_NEWWALLET_HPP_
