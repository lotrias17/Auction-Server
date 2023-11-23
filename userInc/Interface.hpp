#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <string>
#include <vector>
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
    private:
    Client* _client;
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