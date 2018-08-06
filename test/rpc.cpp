/**
 * Copyright (c) 2017-2018 Bitprim Inc.
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

#include <bitprim/rpc/messages.hpp>
#include <bitprim/keoken/state_dto.hpp>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

// Unit Tests ----------------------------------------------------
#ifdef DOCTEST_LIBRARY_INCLUDED

class block_chain_dummy {
public:
    ///// Relay transactions is network setting that is passed through to block
    ///// population as an optimization. This can be removed once there is an
    ///// in-memory cache of tx pool metadata, as the costly query will go away.
    //block_chain(threadpool& pool,
    //	const blockchain::settings& chain_settings,
    //	const database::settings& database_settings,
    //	bool relay_transactions = true);

    ///// The database is closed on destruct, threads must be joined.
    //~block_chain();

    //// ========================================================================
    //// FAST CHAIN
    //// ========================================================================

    //// Readers.
    //// ------------------------------------------------------------------------
    //// Thread safe, unprotected by sequential lock.

    ///// Get the set of block gaps in the chain.
    //bool get_gaps(database::block_database::heights& out_gaps) const;

    ///// Get a determination of whether the block hash exists in the store.
    //bool get_block_exists(const hash_digest& block_hash) const;

    /// Get the hash of the block if it exists.
    bool get_block_hash(libbitcoin::hash_digest& out_hash, size_t height) const {
        return true;
    }

    ///// Get the work of the branch starting at the given height.
    //bool get_branch_work(uint256_t& out_work, const uint256_t& maximum,
    //	size_t height) const;

    ///// Get the header of the block at the given height.
    //bool get_header(chain::header& out_header, size_t height) const;

    ///// Get the height of the block with the given hash.
    //bool get_height(size_t& out_height, const hash_digest& block_hash) const;

    ///// Get the bits of the block with the given height.
    //bool get_bits(uint32_t& out_bits, const size_t& height) const;

    ///// Get the timestamp of the block with the given height.
    //bool get_timestamp(uint32_t& out_timestamp, const size_t& height) const;

    ///// Get the version of the block with the given height.
    //bool get_version(uint32_t& out_version, const size_t& height) const;

    /// Get height of latest block.
    bool get_last_height(size_t& out_height) const {
        return true;
    }

    ///// Get the output that is referenced by the outpoint.
    //bool get_output(chain::output& out_output, size_t& out_height,
    //	uint32_t& out_median_time_past, bool& out_coinbase,
    //	const chain::output_point& outpoint, size_t branch_height,
    //	bool require_confirmed) const;

    //bool get_output_is_confirmed(chain::output& out_output, size_t& out_height,
    //	bool& out_coinbase, bool& out_is_confirmed, const chain::output_point& outpoint,
    //	size_t branch_height, bool require_confirmed) const;

    ///// Determine if an unspent transaction exists with the given hash.
    //bool get_is_unspent_transaction(const hash_digest& hash,
    //	size_t branch_height, bool require_confirmed) const;

    ///// Get position data for a transaction.
    //bool get_transaction_position(size_t& out_height, size_t& out_position,
    //	const hash_digest& hash, bool require_confirmed) const;

    ///////// Get the transaction of the given hash and its block height.
    //////transaction_ptr get_transaction(size_t& out_block_height,
    //////    const hash_digest& hash, bool require_confirmed) const;

    //// Writers.
    //// ------------------------------------------------------------------------
    //// Thread safe, insert does not set sequential lock.

    ///// Create flush lock if flush_writes is true, and set sequential lock.
    //bool begin_insert() const;

    ///// Clear flush lock if flush_writes is true, and clear sequential lock.
    //bool end_insert() const;

    ///// Insert a block to the blockchain, height is checked for existence.
    ///// Reads and reorgs are undefined when chain is gapped.
    //bool insert(block_const_ptr block, size_t height);

    ///// Push an unconfirmed transaction to the tx table and index outputs.
    //void push(transaction_const_ptr tx, dispatcher& dispatch,
    //	result_handler handler);

    ///// Swap incoming and outgoing blocks, height is validated.
    //void reorganize(const config::checkpoint& fork_point,
    //	block_const_ptr_list_const_ptr incoming_blocks,
    //	block_const_ptr_list_ptr outgoing_blocks, dispatcher& dispatch,
    //	result_handler handler);

    //// Properties
    //// ------------------------------------------------------------------------

    /// Get forks chain state relative to chain top.
    libbitcoin::chain::chain_state::ptr chain_state() const {
        return  libbitcoin::chain::chain_state::ptr();
    }

    ///// Get full chain state relative to the branch top.
    //chain::chain_state::ptr chain_state(branch::const_ptr branch) const;

    //// ========================================================================
    //// SAFE CHAIN
    //// ========================================================================
    //// Thread safe.

    //// Startup and shutdown.
    //// ------------------------------------------------------------------------
    //// Thread safe except start.

    ///// Start the block pool and the transaction pool.
    //bool start();

    ///// Signal pool work stop, speeds shutdown with multiple threads.
    //bool stop();

    ///// Unmaps all memory and frees the database file handles.
    ///// Threads must be joined before close is called (or by destruct).
    //bool close();

    //// Node Queries.
    //// ------------------------------------------------------------------------

    /// fetch a block by height.
    // virtual      // OLD previo a merge de Feb2017
    void fetch_block(size_t height, bool witness, libbitcoin::blockchain::safe_chain::block_fetch_handler handler) const {}

    /// fetch a block by hash.
    void fetch_block(const libbitcoin::hash_digest& hash, bool witness, libbitcoin::blockchain::safe_chain::block_fetch_handler handler) const {}

    /// fetch block header by height.
    // virtual      // OLD previo a merge de Feb2017 
    void fetch_block_header(size_t height, libbitcoin::blockchain::safe_chain::block_header_fetch_handler handler) const {}

    void fetch_block_header_txs_size(libbitcoin::hash_digest const& hash, libbitcoin::blockchain::safe_chain::block_header_txs_size_fetch_handler handler) const {}

    void fetch_block_hash_timestamp(size_t height, libbitcoin::blockchain::safe_chain::block_hash_time_fetch_handler handler) const {}

    ///// fetch block header by hash.
    //void fetch_block_header(const hash_digest& hash,
    //	block_header_fetch_handler handler) const;

    ///// fetch hashes of transactions for a block, by block height.
    //// OLD previo a merge de Feb2017 
    //// virtual 
    //// void fetch_merkle_block(size_t height, transaction_hashes_fetch_handler handler) const;
    //void fetch_merkle_block(size_t height, merkle_block_fetch_handler handler) const;

    ///// fetch hashes of transactions for a block, by block hash.
    //void fetch_merkle_block(const hash_digest& hash,
    //	merkle_block_fetch_handler handler) const;

    ///// fetch compact block by block height.
    //void fetch_compact_block(size_t height,
    //	compact_block_fetch_handler handler) const;

    ///// fetch compact block by block hash.
    //void fetch_compact_block(const hash_digest& hash,
    //	compact_block_fetch_handler handler) const;

    ///// fetch height of block by hash.
    //void fetch_block_height(const hash_digest& hash,
    //	block_height_fetch_handler handler) const;

    /// fetch height of latest block.
    void fetch_last_height(libbitcoin::blockchain::safe_chain::last_height_fetch_handler handler) const {}

    /// fetch transaction by hash.
    void fetch_transaction(const libbitcoin::hash_digest& hash, bool require_confirmed, bool witness, libbitcoin::blockchain::safe_chain::transaction_fetch_handler handler) const {}

    ///// Generate fees for mining
    //std::pair<bool, uint64_t> total_input_value(libbitcoin::chain::transaction const& tx) const;
    //std::pair<bool, uint64_t> fees(libbitcoin::chain::transaction const& tx) const;
    //bool is_missing_previous_outputs(chain::transaction const& tx) const;
    ////    bool is_double_spent(chain::transaction const& tx) const;

    //std::pair<bool, size_t> validate_tx(chain::transaction const& tx) const;
    std::vector<libbitcoin::blockchain::block_chain::tx_benefit> get_gbt_tx_list() const {
        return std::vector<libbitcoin::blockchain::block_chain::tx_benefit>();
    }

    void remove_mined_txs_from_chosen_list(libbitcoin::block_const_ptr blk) {
    }

    bool add_to_chosen_list(libbitcoin::transaction_const_ptr tx){
        return true;
    }
    //std::pair<bool, size_t> is_double_spent_and_sigops(chain::transaction const& tx, bool bip16_active) const;
    //std::tuple<bool, size_t, uint64_t> is_double_spent_sigops_and_fees(chain::transaction const& tx, bool bip16_active) const;
    //std::tuple<bool, size_t, uint64_t> validate_tx_2(chain::transaction const& tx, size_t height) const;

    ///// fetch position and height within block of transaction by hash.
    //void fetch_transaction_position(const hash_digest& hash,
    //	bool require_confirmed, transaction_index_fetch_handler handler) const;

    ///// fetch the set of block hashes indicated by the block locator.
    //void fetch_locator_block_hashes(get_blocks_const_ptr locator,
    //	const hash_digest& threshold, size_t limit,
    //	inventory_fetch_handler handler) const;

    ///// fetch the set of block headers indicated by the block locator.
    //void fetch_locator_block_headers(get_headers_const_ptr locator,
    //	const hash_digest& threshold, size_t limit,
    //	locator_block_headers_fetch_handler handler) const;

    ///// fetch a block locator relative to the current top and threshold.
    //void fetch_block_locator(const chain::block::indexes& heights,
    //	block_locator_fetch_handler handler) const;

    //// Server Queries.
    ////-------------------------------------------------------------------------

    ///// fetch the inpoint (spender) of an outpoint.
    void fetch_spend(const libbitcoin::chain::output_point& outpoint, libbitcoin::blockchain::safe_chain::spend_fetch_handler handler) const {}

    /// fetch outputs, values and spends for an address_hash.
    void fetch_history(const libbitcoin::short_hash& address_hash, size_t limit, size_t from_height, libbitcoin::blockchain::safe_chain::history_fetch_handler handler) const {}

    /// Fetch all the txns used by the wallet
    void fetch_confirmed_transactions(const libbitcoin::short_hash& address_hash, size_t limit, size_t from_height, libbitcoin::blockchain::safe_chain::confirmed_transactions_fetch_handler handler) const {}

    std::vector<libbitcoin::blockchain::mempool_transaction_summary> get_mempool_transactions(std::vector<std::string> const& payment_addresses, bool use_testnet_rules, bool witness) const {
        return std::vector<libbitcoin::blockchain::mempool_transaction_summary> ();
    }

    ///// fetch stealth results.
    //void fetch_stealth(const binary& filter, size_t from_height,
    //	stealth_fetch_handler handler) const;

    //// Transaction Pool.
    ////-------------------------------------------------------------------------

    ///// Fetch a merkle block for the maximal fee block template.
    //void fetch_template(merkle_block_fetch_handler handler) const;

    ///// Fetch an inventory vector for a rational "mempool" message response.
    //void fetch_mempool(size_t count_limit, uint64_t minimum_fee,
    //	inventory_fetch_handler handler) const;

    //// Filters.
    ////-------------------------------------------------------------------------

    ///// Filter out block by hash that exist in the block pool or store.
    //void filter_blocks(get_data_ptr message, result_handler handler) const;

    ///// Filter out confirmed and unconfirmed transactions by hash.
    //void filter_transactions(get_data_ptr message,
    //	result_handler handler) const;

    //// Subscribers.
    ////-------------------------------------------------------------------------

    ///// Subscribe to blockchain reorganizations, get branch/height.
    //void subscribe_blockchain(reorganize_handler&& handler);

    ///// Subscribe to memory pool additions, get transaction.
    //void subscribe_transaction(transaction_handler&& handler);

    ///// Send null data success notification to all subscribers.
    //void unsubscribe();

    //// Organizers.
    ////-------------------------------------------------------------------------

    ///// Organize a block into the block pool if valid and sufficient.
    void organize(libbitcoin::block_const_ptr block, libbitcoin::blockchain::safe_chain::result_handler handler) {}

    ///// Store a transaction to the pool if valid.
    void organize(libbitcoin::transaction_const_ptr tx, libbitcoin::blockchain::safe_chain::result_handler handler) {}

    //// Properties.
    ////-------------------------------------------------------------------------

    //bool is_stale() const;
    //const settings& chain_settings() const;

};

class keoken_manager_dummy {
public:
    void initialize_from_blockchain(){};

    std::vector<bitprim::keoken::get_assets_data> get_assets_by_address(bc::wallet::payment_address const& addr) const {
        return {};
    };

    std::vector<bitprim::keoken::get_assets_data> get_assets() const {
        return {};
    };

    std::vector<bitprim::keoken::get_all_asset_addresses_data> get_all_asset_addresses() const {
        return {};    
    };

};

class full_node_dummy {
public:
    // libbitcoin::network::settings p2p_settings_;
    // libbitcoin::node::settings node_settings_;

#ifdef WITH_KEOKEN
    keoken_manager_dummy keoken_manager_;
#endif    
    block_chain_dummy blockchain_;

    block_chain_dummy& chain_bitprim() {
        return blockchain_;
    }

    size_t connection_count() const {
        return 0;
    }

    const libbitcoin::network::settings& network_settings() const {
        return libbitcoin::network::settings{};
    }


    const libbitcoin::node::settings& node_settings() const {
        return libbitcoin::node::settings{};
    }

// #ifdef WITH_KEOKEN
//     keoken_manager_dummy& keoken_manager() {
//         return keoken_manager_;
//     }
// #endif    

};

TEST_CASE("[load_signature_map] validate map keys") {

    auto map = bitprim::load_signature_map<libbitcoin::blockchain::block_chain>();

    CHECK(map.count("getrawtransaction") == 1);
    CHECK(map.count("getaddressbalance") == 1);
    CHECK(map.count("getspentinfo") == 1);
    CHECK(map.count("getaddresstxids") == 1);
    CHECK(map.count("getaddressdeltas") == 1);
    CHECK(map.count("getaddressutxos") == 1);
    CHECK(map.count("getblockhashes") == 1);
    CHECK(map.count("getaddressmempool") == 1);
    CHECK(map.count("getblock") == 1);
    CHECK(map.count("getblockhash") == 1);
    CHECK(map.count("getblockheader") == 1);
    CHECK(map.count("validateaddress") == 1);
    CHECK(map.count("getblocktemplate") == 1);

    CHECK(map.count("getbestblockhash") == 0);
    CHECK(map.count("getblockchaininfo") == 0);
    CHECK(map.count("getblockcount") == 0);
    CHECK(map.count("getdifficulty") == 0);
    CHECK(map.count("getchaintips") == 0);
    CHECK(map.count("getmininginfo") == 0);

    CHECK(map.count("submitblock") == 0);
    CHECK(map.count("sendrawtransaction") == 0);
    CHECK(map.count("getinfo") == 0);
}

TEST_CASE("[process_data] invalid key") {

    //using blk_t = libbitcoin::blockchain::block_chain;
    using blk_t = block_chain_dummy;

    auto map = bitprim::load_signature_map<blk_t>();
    auto map_no_params = bitprim::load_signature_map_no_params<blk_t>();
    nlohmann::json input;

    input["method"] = "invalid_key";

    //libbitcoin::threadpool threadpool;
    //libbitcoin::blockchain::settings chain_settings;
    //libbitcoin::database::settings database_settings;

    //libbitcoin::blockchain::block_chain chain(threadpool, chain_settings, database_settings, true);

    //blk_t chain(threadpool, chain_settings, database_settings, true);
    full_node_dummy node;

#ifdef WITH_KEOKEN
    keoken_manager_dummy manager;
    auto ret = bitprim::process_data(input, false, node, manager, map, map_no_params);
#else
    auto ret = bitprim::process_data(input, false, node, map, map_no_params);
#endif    


    nlohmann::json output = nlohmann::json::parse(ret);
    CHECK((int)output["error"]["code"] == bitprim::RPC_INVALID_REQUEST);
}


TEST_CASE("[process_data] getrawtransaction error invalid params") {

    using blk_t = block_chain_dummy;

    auto map = bitprim::load_signature_map<blk_t>();
    auto map_no_params = bitprim::load_signature_map_no_params<blk_t>();
    nlohmann::json input;

    input["method"] = "getrawtransaction";
    input["id"] = 123;
    input["params"] = nullptr;

    full_node_dummy node;

#ifdef WITH_KEOKEN
    keoken_manager_dummy manager;
    auto ret = bitprim::process_data(input, false, node, manager, map, map_no_params);
#else
    auto ret = bitprim::process_data(input, false, node, map, map_no_params);
#endif    

    //MESSAGE(ret);
    
    nlohmann::json output = nlohmann::json::parse(ret);

    CHECK(output["id"] == input["id"]);
    CHECK((int)output["error"]["code"] == bitprim::RPC_PARSE_ERROR);
}



TEST_CASE("[process_data] submitblock ") {

    using blk_t = block_chain_dummy;

    auto map = bitprim::load_signature_map<blk_t>();
    auto map_no_params = bitprim::load_signature_map_no_params<blk_t>();

    nlohmann::json input;

    input["method"] = "submitblock";
    input["id"] = 123;
    input["params"] = nullptr;

    full_node_dummy node;


#ifdef WITH_KEOKEN
    keoken_manager_dummy manager;
    auto ret = bitprim::process_data(input, false, node, manager, map, map_no_params);
#else
    auto ret = bitprim::process_data(input, false, node, map, map_no_params);
#endif    

    //MESSAGE(ret);

    nlohmann::json output = nlohmann::json::parse(ret);

    CHECK(output["id"] == input["id"]);
    CHECK((int)output["error"]["code"] == bitprim::RPC_MISC_ERROR);
}


#ifdef WITH_KEOKEN

TEST_CASE("[process_data] create asset ") {

    using blk_t = block_chain_dummy;

    auto map = bitprim::load_signature_map<blk_t>();
    auto map_no_params = bitprim::load_signature_map_no_params<blk_t>();

    nlohmann::json input, params, origin;

    input["method"] = "createasset";
    input["id"] = 123;
    input["params"]["origin"][0]["output_hash"] = std::string("980de6ce12c29698d54323c6b0f358e1a9ae867598b840ee0094b9df22b07393");
    input["params"]["origin"][0]["output_index"] = 1;
    input["params"]["utxo_satoshis"] = 21647102398;
    input["params"]["asset_name"] = "testcoin";
    input["params"]["asset_amount"] = 1;
    input["params"]["asset_owner"] = "mwx2YDHgpdfHUmCpFjEi9LarXf7EkQN6YG";
    input["params"]["utxo_satoshis"] = 21647102398;


    full_node_dummy node;

#ifdef WITH_KEOKEN
    keoken_manager_dummy manager;
    auto ret = bitprim::process_data(input, false, node, manager, map, map_no_params);
#else
    auto ret = bitprim::process_data(input, false, node, map, map_no_params);
#endif    

    nlohmann::json output = nlohmann::json::parse(ret);

    CHECK(output["id"] == input["id"]);
    CHECK(output["result"] == "01000000019373b022dfb99400ee40b8987586aea9e158f3b0c62343d59896c212cee60d980100000000ffffffff02ee89440a050000001976a914b43ff4532569a00bcab4ce60f87cdeebf985b69a88ac00000000000000001c6a0400004b50150000000074657374636f696e00000000000000000100000000");
}


TEST_CASE("[process_data] send token ") {

    using blk_t = block_chain_dummy;

    auto map = bitprim::load_signature_map<blk_t>();
    auto map_no_params = bitprim::load_signature_map_no_params<blk_t>();

    nlohmann::json input, params, origin;

    input["method"] = "sendtoken";
    input["id"] = 123;
    input["params"]["origin"][0]["output_hash"] = std::string("980de6ce12c29698d54323c6b0f358e1a9ae867598b840ee0094b9df22b07393");
    input["params"]["origin"][0]["output_index"] = 1;
    input["params"]["utxo_satoshis"] = 21647102398;
    input["params"]["asset_id"] = 1;
    input["params"]["asset_amount"] = 1;
    input["params"]["asset_owner"] = "mwx2YDHgpdfHUmCpFjEi9LarXf7EkQN6YG";
    input["params"]["token_receiver"] = "ms9qrYaJ468QCwgX6v7ittgX3vBk3mg6PM";
    input["params"]["dust"] = 20000;
    input["params"]["utxo_satoshis"] = 21647102398;


    full_node_dummy node;

#ifdef WITH_KEOKEN
    keoken_manager_dummy manager;
    auto ret = bitprim::process_data(input, false, node, manager, map, map_no_params);
#else
    auto ret = bitprim::process_data(input, false, node, map, map_no_params);
#endif    

    nlohmann::json output = nlohmann::json::parse(ret);

    CHECK(output["id"] == input["id"]);
    CHECK(output["result"] == "01000000019373b022dfb99400ee40b8987586aea9e158f3b0c62343d59896c212cee60d980100000000ffffffff03204e0000000000001976a9147fa36605e302ed00aeca0da8e2743772df11290188acce3b440a050000001976a914b43ff4532569a00bcab4ce60f87cdeebf985b69a88ac0000000000000000176a0400004b50100000000100000001000000000000000100000000");
}


#endif


#endif /*DOCTEST_LIBRARY_INCLUDED*/

