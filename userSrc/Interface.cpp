#include "../userInc/user.hpp"

Interface::Interface() {
    // this value should be the group number but it is 11 for the tejo testing
    _port = 58011;
    strcpy(_server, "localhost");
    _client = NULL;
}

Interface::Interface(int port) {
    _port = port;
    strcpy(_server, "localhost");
    _client = NULL;
}

Interface::Interface(char* server) {
    _port = 58011;
    strcpy(_server, server);
    _client = NULL;
}

Interface::Interface(int port, char* server) {
    _port = port;
    strcpy(_server, server);
    _client = NULL;
}

int Interface::processInput() {
    _words.clear();
    int j = _input.size();
    int iword = 0;
    _nWords = 0;
    char word[50];
    for (int i = 0; i < j; i++) {
        if (_input[i] == ' ') {
            word[iword] = '\0';
            iword = 0;
            if (word[0] == '\0') {
                cout << "Double Space!\n";
                return -1;
            }
            _words.push_back(word);
            _nWords++;
        }
        else {
            if (iword > 49) {
                cout << "Output muito grande!\n";
                return -1;
            }
            word[iword] = _input[i];
            iword++;
        }
    }
    word[iword] = '\0';
    if (word[0] == '\0') {
        cout << "Double Space!\n";
        return -1;
    }
    _words.push_back(word);
    _nWords++;
    return 0;
}

int Interface::getInput() {
    cout << "> ";
    getline(cin, _input);

    return processInput();
}

bool Interface::isNumeric(string str) {
    int size = (int) str.size();
    for (int i = 0; i < size; i++) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

bool Interface::isAlphaNumeric(string str) {
    int size = (int) str.size();
    for (int i = 0; i < size; i++) {
        if (!isalnum(str[i])) {
            return false;
        }
    }
    return true;
}

bool Interface::checkUIDFormat(string str) {
    return str.size() == 6 && isNumeric(str);
}

bool Interface::checkpasswordFormat(string str) {
    return str.size() == 8 && isAlphaNumeric(str);
}

string Interface::toString() {
    string output = "Pelo PORT: " + to_string(_port) + "\nPelo servidor: " + _server + "\nO ultimo input foi: " + _input + "\n";
    for (int i = 1; i <= _nWords; i++) {
        output = output + "Palavra numero " + to_string(i) + " é: " + _words[i - 1] + "\n";
    }
    return output;
}

int Interface::exec() {
    // returns -1 if we should exit the app and 0 if continue
    if (_words[0] == "exit") {
        if (_nWords != 1) {
            cout << "Comando mal inserido: exit\n";
            return 0;
        } else {
            if (_client != NULL) {
                cout << "Tem que dar logout antes de sair da aplicação!\n";
                return 0;
            }
            cout << "De resto um bom dia!\n";
            return -1;
        }
    } else {
    if (_words[0] == "login") {
        if (_nWords != 3) {
            cout << "Comando mal inserido: login UID(6 digits) password(8 alphanumeric)\n";
            return 0;
        }
        else {
            if (checkUIDFormat(_words[1])) {
                if (checkpasswordFormat(_words[2])) {
                    // do the login
                    _client = new Client(_words[1], _words[2]);
                    cout << _client->toString();
                    return 0;
                } else {
                    cout << "escreveste password fraca\n";
                    return 0;
                }
            } else {
                cout << "escreveste mal o UID\n";
                return 0;
            }
            return 0;
        }
    } else {
    if (_words[0] == "logout") {
        if (_nWords != 1) {
            cout << "Comando mal inserido: logout\n";
            return 0;
        } else {
            if (_client == NULL) {
                std::cout << "ja esta logged out\n";
                return 0;
            }
            cout << "logging out!\n";
            delete _client;
            _client = NULL;
            return 0;
        }
    } else {
        cout << "comando invalido!\n";
        return 0;
    }
    }
    }
    return 0;
}