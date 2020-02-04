/**
* Copyright (c) 2016-2020 Knuth Project developers.
*
* This file is part of kth-node.
*
* kth-node is free software: you can redistribute it and/or
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

#ifndef KTH_RPC_MESSAGES_MESSAGES_HPP_
#define KTH_RPC_MESSAGES_MESSAGES_HPP_

#if (defined(KTH_DB_LEGACY) && defined(KTH_DB_SPENDS) && defined(KTH_DB_HISTORY)) || defined(KTH_DB_NEW_FULL)
#include <knuth/rpc/messages/blockchain/getaddressbalance.hpp>
#endif

#if (defined(KTH_DB_LEGACY) && defined(KTH_DB_SPENDS)) || defined(KTH_DB_NEW_FULL)
#include <knuth/rpc/messages/blockchain/getrawtransaction.hpp>
#include <knuth/rpc/messages/blockchain/getspentinfo.hpp>
#endif

#if defined(KTH_DB_TRANSACTION_UNCONFIRMED) || defined(KTH_DB_NEW_FULL)
#include <knuth/rpc/messages/blockchain/getaddressmempool.hpp>
#include <knuth/rpc/messages/blockchain/getaddresstxids.hpp>
#endif

#if (defined(KTH_DB_LEGACY) && defined(KTH_DB_SPENDS) && defined(KTH_DB_HISTORY)) || defined(KTH_DB_NEW_FULL)
#include <knuth/rpc/messages/blockchain/getaddressdeltas.hpp>
#include <knuth/rpc/messages/blockchain/getaddressutxos.hpp>
#endif

#include <knuth/rpc/messages/blockchain/getbestblockhash.hpp>

#include <knuth/rpc/messages/blockchain/getblockchaininfo.hpp>

#if defined(KTH_DB_LEGACY) || defined(KTH_DB_NEW_BLOCKS) || defined(KTH_DB_NEW_FULL) 
#include <knuth/rpc/messages/blockchain/getblockhashes.hpp>
#include <knuth/rpc/messages/blockchain/getblock.hpp>
#include <knuth/rpc/messages/blockchain/getchaintips.hpp>
#include <knuth/rpc/messages/blockchain/getblockheader.hpp>
#endif

#if defined(KTH_DB_LEGACY) || defined(KTH_DB_NEW) 
#include <knuth/rpc/messages/util/getinfo.hpp>
#include <knuth/rpc/messages/mining/getmininginfo.hpp>
#include <knuth/rpc/messages/blockchain/getdifficulty.hpp>
#endif // defined(KTH_DB_LEGACY) || defined(KTH_DB_NEW) 


#include <knuth/rpc/messages/blockchain/getblockhash.hpp>
#include <knuth/rpc/messages/blockchain/getblockcount.hpp>

#ifdef KTH_WITH_MEMPOOL
#include <knuth/rpc/messages/mining/getblocktemplate.hpp>
#endif

#include <knuth/rpc/messages/mining/submitblock.hpp>
#include <knuth/rpc/messages/wallet/sendrawtransaction.hpp>

#include <knuth/rpc/messages/util/validateaddress.hpp>
#include <knuth/rpc/messages/util/getnetworkinfo.hpp>
#include <knuth/rpc/messages/wallet/createtransaction.hpp>
#include <knuth/rpc/messages/wallet/createsignature.hpp>
#include <knuth/rpc/messages/wallet/setsignature.hpp>
#include <knuth/rpc/messages/wallet/newwallet.hpp>

#ifdef KTH_WITH_KEOKEN
#include <knuth/rpc/messages/keoken/createasset.hpp>
#include <knuth/rpc/messages/keoken/sendtoken.hpp>
#include <knuth/rpc/messages/keoken/initkeoken.hpp>
#include <knuth/rpc/messages/keoken/getassets.hpp>
#include <knuth/rpc/messages/keoken/getassetsbyaddress.hpp>
#include <knuth/rpc/messages/keoken/getallassets.hpp>
#include <knuth/rpc/messages/keoken/getkeokenaddress.hpp>
#include <knuth/rpc/messages/keoken/getkeokenblock.hpp>
#endif //KTH_WITH_KEOKEN

#endif //KTH_RPC_MESSAGES_MESSAGES_HPP_
