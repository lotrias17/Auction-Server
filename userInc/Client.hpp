#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>

using namespace std;

class Client {
    public:
    Client(string UID, string password);
    string _UID;
    string _password;
    string toString();
    private:
};

#endif