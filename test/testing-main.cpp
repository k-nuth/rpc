// Copyright (c) 2016-2020 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <iostream>
#include <kth/blockchain.hpp>
#include <kth/rpc/zmq/zmq_helper.hpp>

int main () {
    kth::threadpool t(0);
    const kth::blockchain::settings s;
    const kth::database::settings d;
    kth::blockchain::block_chain chain(t, s, d);

    kth::rpc::zmq zmq_object(5556, chain);

    size_t i = 1;
    while (i<=5){
        if (zmq_object.send_random_data()){
            std::cout << "Message sent" << i << std::endl;
        }
        ++i;
    }

    zmq_object.close();

    std::cout << "program ended" << std::endl;
    return 1;
}