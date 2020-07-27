// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef KTH_RPC_MESSAGES_MESSAGES_HPP_
#define KTH_RPC_MESSAGES_MESSAGES_HPP_

#if (defined(KTH_DB_LEGACY) && defined(KTH_DB_SPENDS) && defined(KTH_DB_HISTORY)) || defined(KTH_DB_NEW_FULL)
#include <kth/rpc/messages/blockchain/getaddressbalance.hpp>
#endif

#if (defined(KTH_DB_LEGACY) && defined(KTH_DB_SPENDS)) || defined(KTH_DB_NEW_FULL)
#include <kth/rpc/messages/blockchain/getrawtransaction.hpp>
#include <kth/rpc/messages/blockchain/getspentinfo.hpp>
#endif

#if defined(KTH_DB_TRANSACTION_UNCONFIRMED) || defined(KTH_DB_NEW_FULL)
#include <kth/rpc/messages/blockchain/getaddressmempool.hpp>
#include <kth/rpc/messages/blockchain/getaddresstxids.hpp>
#endif

#if (defined(KTH_DB_LEGACY) && defined(KTH_DB_SPENDS) && defined(KTH_DB_HISTORY)) || defined(KTH_DB_NEW_FULL)
#include <kth/rpc/messages/blockchain/getaddressdeltas.hpp>
#include <kth/rpc/messages/blockchain/getaddressutxos.hpp>
#endif

#include <kth/rpc/messages/blockchain/getbestblockhash.hpp>

#include <kth/rpc/messages/blockchain/getblockchaininfo.hpp>

#if defined(KTH_DB_LEGACY) || defined(KTH_DB_NEW_BLOCKS) || defined(KTH_DB_NEW_FULL) 
#include <kth/rpc/messages/blockchain/getblockhashes.hpp>
#include <kth/rpc/messages/blockchain/getblock.hpp>
#include <kth/rpc/messages/blockchain/getchaintips.hpp>
#include <kth/rpc/messages/blockchain/getblockheader.hpp>
#endif

#if defined(KTH_DB_LEGACY) || defined(KTH_DB_NEW) 
#include <kth/rpc/messages/util/getinfo.hpp>
#include <kth/rpc/messages/mining/getmininginfo.hpp>
#include <kth/rpc/messages/blockchain/getdifficulty.hpp>
#endif // defined(KTH_DB_LEGACY) || defined(KTH_DB_NEW) 


#include <kth/rpc/messages/blockchain/getblockhash.hpp>
#include <kth/rpc/messages/blockchain/getblockcount.hpp>

#ifdef KTH_WITH_MEMPOOL
#include <kth/rpc/messages/mining/getblocktemplate.hpp>
#endif

#include <kth/rpc/messages/mining/submitblock.hpp>
#include <kth/rpc/messages/wallet/sendrawtransaction.hpp>

#include <kth/rpc/messages/util/validateaddress.hpp>
#include <kth/rpc/messages/util/getnetworkinfo.hpp>
#include <kth/rpc/messages/wallet/createtransaction.hpp>
#include <kth/rpc/messages/wallet/createsignature.hpp>
#include <kth/rpc/messages/wallet/setsignature.hpp>
#include <kth/rpc/messages/wallet/newwallet.hpp>

#ifdef KTH_WITH_KEOKEN
#include <kth/rpc/messages/keoken/createasset.hpp>
#include <kth/rpc/messages/keoken/sendtoken.hpp>
#include <kth/rpc/messages/keoken/initkeoken.hpp>
#include <kth/rpc/messages/keoken/getassets.hpp>
#include <kth/rpc/messages/keoken/getassetsbyaddress.hpp>
#include <kth/rpc/messages/keoken/getallassets.hpp>
#include <kth/rpc/messages/keoken/getkeokenaddress.hpp>
#include <kth/rpc/messages/keoken/getkeokenblock.hpp>
#endif //KTH_WITH_KEOKEN

#endif //KTH_RPC_MESSAGES_MESSAGES_HPP_
