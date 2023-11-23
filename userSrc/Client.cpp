#include "../userInc/user.hpp"

using namespace std;

Client::Client(string UID, string password) {
    _UID = UID;
    _password = password;
}

string Client::toString() {
    return "Utilizador logged in: " + _UID + " | " + _password + "\n";
}