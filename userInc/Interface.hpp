#ifndef INTERFACE_HPP
#define INTERFACE_HPP

//#include <string>
#include <unistd.h>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "Client.hpp"


using namespace std;

class Interface {
    // add port with args...
    public:
    Interface(int port, char* server);
    Interface(char* server);
    Interface(int port);
    Interface();
    int getInput();
    string toString();
    int exec();
    void prepareSocket();
    private:
    int login(); // falta ainda adicionar o check do que aconteceu apos madnar esta msg
    int logout(); // falta ...
    int _fd, _errcode;
    socklen_t _addrlen;
    struct addrinfo _hints, *_res;
    struct sockaddr_in _addr;
    char _buffer[8 * 1024];
    Client* _client; // so criar isto se realmente deu login
    int _port;
    char _server[256];
    string _input;
    int _nWords;
    vector<string> _words;
    int processInput();
    bool isNumeric(string str);
    bool isAlphaNumeric(string str);
    bool checkUIDFormat(string str);
    bool checkpasswordFormat(string str);
};

#endif