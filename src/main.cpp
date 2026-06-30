#include "p2p/net/node.hpp"

#define LH_A "localhost"
#define LH_P "9876"

#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <cerrno>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>

int main(int argc, char *argv[]) {
    (void)argc;

    Node node("localhost", argv[1]);

    getchar();
    return 0;
}
