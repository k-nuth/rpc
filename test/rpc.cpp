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

#include <bitprim/rpc/messages.hpp>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

// Unit Tests ----------------------------------------------------
#ifdef DOCTEST_LIBRARY_INCLUDED


TEST_CASE("[load_signature_map] validate map keys") {
    
	auto map = bitprim::load_signature_map();

	CHECK(map.count("getrawtransaction") == 1);
	CHECK(map.count("getaddressbalance") == 1);
	CHECK(map.count("getspentinfo") == 1);
	CHECK(map.count("getaddresstxids") == 1);
	CHECK(map.count("getaddressdeltas") == 1);
	CHECK(map.count("getaddressutxos") == 1);
	CHECK(map.count("getblockhashes") == 1);
	CHECK(map.count("getinfo") == 1);
	CHECK(map.count("getaddressmempool") == 1);
	CHECK(map.count("getbestblockhash") == 1);
	CHECK(map.count("getblock") == 1);
	CHECK(map.count("getblockhash") == 1);
	CHECK(map.count("getblockchaininfo") == 1);
	CHECK(map.count("getblockheader") == 1);
	CHECK(map.count("getblockcount") == 1);
	CHECK(map.count("getdifficulty") == 1);
	CHECK(map.count("getchaintips") == 1);
	CHECK(map.count("validateaddress") == 1);
	CHECK(map.count("getblocktemplate") == 1);
	CHECK(map.count("getmininginfo") == 1);
	
	CHECK(map.count("submitblock") == 0);
	CHECK(map.count("sendrawtransaction") == 0);

}

#endif /*DOCTEST_LIBRARY_INCLUDED*/

