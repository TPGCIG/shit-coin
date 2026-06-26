#include "p2p/net/node.hpp"
#include "p2p/net/dispatcher.hpp"
#include "p2p/peers.hpp"

#include <asm-generic/socket.h>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <cerrno>

int main(int argc, char* argv[]) {
        
    (void)argc;
    (void)argv;

    Dispatcher dispatcher;
    PeerList pl;

    int op = pl.retrieve_central_peers();

    std::cout << op;


    std::cout << "end main" << std::endl;






    return 0;




}
