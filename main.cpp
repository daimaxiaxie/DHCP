#include "header.h"

//#include <sys/socket.h>
//#include <sys/types.h>

void UDPListen() {
    int listenfd = socket(PF_INET, SOCK_DGRAM, 0);
}

int main(int argc, char **argv) {
    Init();
    return 0;
}
