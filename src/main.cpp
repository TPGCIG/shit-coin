#include "p2p/net/listener.hpp"

#include <asm-generic/socket.h>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <thread>
#include <unistd.h>
#include <cerrno>

int main(int argc, char* argv[]) {
        
    (void)argc;
    (void)argv;

    NetworkListener incoming{};

    std::thread listen_thread(&NetworkListener::start_listening, &incoming);


    listen_thread.join();
    
    

    std::cout << "end main" << std::endl;






    return 0;




}
