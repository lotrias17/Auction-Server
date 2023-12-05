#include "../asInc/AS.hpp"

using namespace std;

void udpServer() {
    string s;

    fd=socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        cerr << "Problema no socket UDP.\n";
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    errcode=getaddrinfo(NULL, PORT, &hints, &res);
    if (errcode != 0) {
        cerr << "Problema no getaddrinfo.\n";
        exit(1);
    }

    n=bind(fd, res->ai_addr, res->ai_addrlen);
    if (n == -1) {
        cerr << "Problema no bind.\n";
        exit(1);
    }

    while(1) {
        cin >> s;
        if (s == "exit") break;

        addrlen = sizeof(addr);
        n=recvfrom(fd,buffer,128,0,(struct sockaddr*) &addr, &addrlen);
        if (n==-1) {
            cout << "Problema no recvfrom.\n";
        }
        write(1, "received: ", 10); write(1, buffer, n);
        n=sendto(fd,buffer,n,0,(struct sockaddr*) &addr, addrlen);
        if (n==-1) {
            cout << "Problema no sendto.\n";
        }
    }
    return;
}

int main() {

    cout << "Ola meu servidor!\n";

    udpServer();

    freeaddrinfo(res);
    close(fd);
}