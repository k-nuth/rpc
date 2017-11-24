/**
 * Copyright (c) 2017 Bitprim developers (see AUTHORS)
 *
 * This file is part of Bitprim.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BITPRIM_RPC_MESSAGES_HPP_
#define BITPRIM_RPC_MESSAGES_HPP_

#include <bitprim/rpc/json/json.hpp>
#include <bitcoin/blockchain/interface/block_chain.hpp>

namespace bitprim {
    // First message:
    std::string process_data(nlohmann::json const& json_object, bool use_testnet_rules, libbitcoin::blockchain::block_chain& chain);


    // RPC
    // ------------------------------------------------------------------------
    nlohmann::json getrawtransaction (std::string const& txid, const bool verbose, libbitcoin::blockchain::block_chain const& chain, bool use_testnet_rules = false);
    nlohmann::json getspentinfo (std::string const& txid, size_t const& index, libbitcoin::blockchain::block_chain const& chain);
    nlohmann::json getaddresstxids(std::vector<std::string> const& address, size_t const& start_height, size_t const& end_height, libbitcoin::blockchain::block_chain const& chain);
    nlohmann::json getaddressdeltas(std::vector<std::string> const& addresses, size_t const& start_height, size_t const& end_height, const bool include_chain_info, libbitcoin::blockchain::block_chain const& chain);
    nlohmann::json getaddressbalance (std::vector<std::string> const& addresses, libbitcoin::blockchain::block_chain const& chain);
    nlohmann::json getaddressutxos(std::vector<std::string> const& address, const bool chain_info, libbitcoin::blockchain::block_chain const& chain);

    nlohmann::json getblockhashes(uint32_t time_high, uint32_t time_low, bool no_orphans, bool logical_times, libbitcoin::blockchain::block_chain const& chain);
    void update_mid(size_t top_height, size_t low_height, size_t& mid, libbitcoin::message::header::ptr& header);
    void getblockheader(size_t i,libbitcoin::message::header::ptr& header, libbitcoin::blockchain::block_chain const& chain);

    std::tuple<bool, size_t, double> get_last_block_difficulty();
    nlohmann::json getinfo(libbitcoin::blockchain::block_chain const& chain);

    nlohmann::json getaddressmempool(std::vector<std::string> const& addresses, libbitcoin::blockchain::block_chain const& chain);
    nlohmann::json getbestblockhash(libbitcoin::blockchain::block_chain const& chain);
    nlohmann::json getblock(const std::string & block_hash, bool verbose, libbitcoin::blockchain::block_chain const& chain);
    nlohmann::json getblockhash(const size_t height, libbitcoin::blockchain::block_chain const& chain);
    nlohmann::json getblockchaininfo(libbitcoin::blockchain::block_chain const& chain);
    nlohmann::json rpc_getblockheader(const std::string & block_hash, bool verbose, libbitcoin::blockchain::block_chain const& chain);

    nlohmann::json getblockcount(libbitcoin::blockchain::block_chain const& chain);
    nlohmann::json getdifficulty(libbitcoin::blockchain::block_chain const& chain);
    nlohmann::json getchaintips(libbitcoin::blockchain::block_chain const& chain);
    nlohmann::json validateaddress(std::string& raw_address, libbitcoin::blockchain::block_chain const& chain);
    nlohmann::json getblocktemplate(bool use_testnet_rules, size_t max_bytes, std::chrono::nanoseconds timeout, libbitcoin::blockchain::block_chain const& chain);
    nlohmann::json submitblock(bool use_testnet_rules, std::string const& incoming_hex, libbitcoin::blockchain::block_chain& chain);
    nlohmann::json sendrawtransaction(bool use_testnet_rules, std::string const & incoming_hex, bool allowhighfees,libbitcoin::blockchain::block_chain& chain);
    nlohmann::json getmininginfo(bool use_testnet_rules, libbitcoin::blockchain::block_chain const& chain);


    //JSON IN
    //-------------------------------------------------------------------
    bool json_in_getrawtransaction(nlohmann::json const& json_object, std::string& tx_id, bool& verbose);
    bool json_in_getaddressbalance(nlohmann::json const& json_object, std::vector<std::string>& address);
    bool json_in_getspentinfo(nlohmann::json const& json_object, std::string& tx_id, size_t& index);
    bool json_in_getaddresstxids(nlohmann::json const& json_object, std::vector<std::string>& payment_address, size_t& start_height, size_t& end_height);
    bool json_in_getaddressdeltas(nlohmann::json const& json_object, std::vector<std::string>& payment_address, size_t& start_height, size_t& end_height, bool& include_chain_info);
    bool json_in_getaddressutxos(nlohmann::json const& json_object, std::vector<std::string>& payment_address, bool& chain_info);
    bool json_in_getblockhashes(nlohmann::json const& json_object, uint32_t& time_high, uint32_t& time_low, bool& no_orphans, bool& logical_times);
    bool json_in_getaddressmempool(nlohmann::json const& json_object, std::vector<std::string>& payment_address);
    bool json_in_getblock(nlohmann::json const& json_object, std::string & hash, bool & verbose);
    bool json_in_getblockhash(nlohmann::json const& json_object, size_t& height);
    bool json_in_getblockheader(nlohmann::json const& json_object, std::string & hash, bool & verbose);
    bool json_in_validateaddress(nlohmann::json const& json_object, std::string& raw_address);
    bool json_in_getblocktemplate(nlohmann::json const& json_object);
    bool json_in_submitblock(nlohmann::json const& json_object, std::string& block_hex_str);
    bool json_in_sendrawtransaction(nlohmann::json const& json_object, std::string& tx_str, bool & allowhighfees);

    //JSON OUT
    //-------------------------------------------------------------------
    nlohmann::json json_out(nlohmann::json const& json_in, nlohmann::json const& json_resp);
    void copy_id(nlohmann::json const& json_in, nlohmann::json & json_resp);
}

#endif //BITPRIM_RPC_MESSAGES_HPP_
