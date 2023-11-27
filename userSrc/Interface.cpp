#include "../userInc/user.hpp"

void Interface::prepareSocket() {
    _fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (_fd == -1) {
        cerr << "Problema ao criar o socket!\n";
        exit(1);
    }

    memset(&_hints, 0, sizeof(_hints));
    _hints.ai_family=AF_INET;
    _hints.ai_socktype=SOCK_DGRAM;

    //cout << "Estou a tentar conectar ao sv: \"" << _server << "\"!\n";
    //cout << "Ao porto: \"" << (char*)to_string(_port).c_str() << "\"!\n";

    _errcode=getaddrinfo(_server, (char*)to_string(_port).c_str(), &_hints, &_res);
    if(_errcode==-1) {
        cerr << "Problema no addr info\n";
        exit(1);
    }
    return;
}

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
    prepareSocket();
}

Interface::Interface(char* server) {
    _port = 58011;
    strcpy(_server, server);
    _client = NULL;
    prepareSocket();
}

Interface::Interface(int port, char* server) {
    _port = port;
    strcpy(_server, server);
    _client = NULL;
    prepareSocket();
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

int Interface::login() {
    memcpy(_buffer, "LIN", 3);
    memcpy(_buffer + 3, " ", 1);
    memcpy(_buffer + 4, (char*)_words[1].c_str(), 6);
    memcpy(_buffer + 10, " ", 1);
    memcpy(_buffer + 11, (char*)_words[2].c_str(), 8);
    memcpy(_buffer + 19, "\n", 1);
    cout << "Eu enviei: " << _buffer;

    int n=sendto(_fd, _buffer, 20, 0,_res->ai_addr,_res->ai_addrlen);
    if(n==-1) {
        cerr << "Erro no sendto(), login()\n";
        exit(1);
    }

    _addrlen=sizeof(_addr);
    n=recvfrom(_fd,_buffer,128,0,(struct sockaddr*)&_addr,&_addrlen);
    if(n==-1) {
        cerr << "Erro no recvfrom(), login()\n";
        exit(1);
    }
    _buffer[n] = '\0';

    cout << "Eu recebi mas n checkei: " << _buffer;

    return 0;
}

int Interface::logout() {
    memcpy(_buffer, "LOU", 3);
    memcpy(_buffer + 3, " ", 1);
    memcpy(_buffer + 4, (char*)_words[1].c_str(), 6);
    memcpy(_buffer + 10, " ", 1);
    memcpy(_buffer + 11, (char*)_words[2].c_str(), 8);
    memcpy(_buffer + 19, "\n", 1);
    cout << "Eu enviei: " << _buffer;

    int n=sendto(_fd, _buffer, 20, 0,_res->ai_addr,_res->ai_addrlen);
    if(n==-1) {
        cerr << "Erro no sendto(), login()\n";
        exit(1);
    }

    _addrlen=sizeof(_addr);
    n=recvfrom(_fd,_buffer,128,0,(struct sockaddr*)&_addr,&_addrlen);
    if(n==-1) {
        cerr << "Erro no recvfrom(), login()\n";
        exit(1);
    }
    _buffer[n] = '\0';

    cout << "Eu recebi mas n checkei: " << _buffer;

    return 0;
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
                    int n = login();
                    if (n == -1) {
                        std::cerr << "Deu merda no login!\n";
                        return -1;
                    }
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
            int n = logout();
            if (n == -1) {
                std::cerr << "Deu merda no login!\n";
                return -1;
            }
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