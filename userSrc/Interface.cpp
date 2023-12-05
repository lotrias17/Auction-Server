#include "../userInc/user.hpp"

void Interface::prepareSocket() {
    // Socket do UDP
    _udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (_udpfd == -1) {
        cerr << "Problema ao criar o socket!\n";
        exit(1);
    }

    // Socket do TCP

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
    if (!_res) {
        cerr << "Não foi encontrado o servidor que mencionou!\n";
        exit(1);
    }
    return;
}

Interface::Interface() {
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
    _port = 58011;  // this value should be the group number but it is 11 for the tejo testing
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

int Interface::get() {  //logo pode-se apagar acho eu
    cout << "YES: " << _port << ", " << _server << '\n';
    return 0;
}

string Interface::toString() {
    string output = "Pelo PORT: " + to_string(_port) + "\nPelo servidor: " + _server + "\n"; // O ultimo input foi: " + _input + "\n";
    /* for (int i = 1; i <= _nWords; i++) {
        output = output + "Palavra numero " + to_string(i) + " é: " + _words[i - 1] + "\n";
    } */
    return output;
}

bool checkServerAnswer(int bufSize, char* buf, string corr) {
    int strSize = corr.length();
    // cout << "checkServerAnswer: " + to_string(bufSize) + " - " + to_string(strSize) + "\n";
    if (bufSize < strSize) {
        return false;
    }

    for (int i = 0; i < strSize; i++) {
        if (buf[i] != corr[i]) {
            return false;
        }
    }    

    return true;
}

int Interface::udpBufferProtocol(int sendSize, int rcvSize) {

    int n = sendto(_udpfd, _buffer, sendSize, 0,_res->ai_addr,_res->ai_addrlen);
    if(n==-1) {
        cerr << "Erro no sendto(), login()\n";
        exit(1);
    }

    _addrlen=sizeof(_addr);
    n=recvfrom(_udpfd,_buffer,rcvSize,0,(struct sockaddr*)&_addr,&_addrlen);
    
    if(n==-1) {
        cerr << "Erro no recvfrom(), login()\n";
        exit(1);
    }
    _buffer[n] = '\0';
    return n;
}

int Interface::login() {
    memcpy(_buffer, "LIN", 3);
    memcpy(_buffer + 3, " ", 1);
    memcpy(_buffer + 4, (char*)_words[1].c_str(), 6);
    memcpy(_buffer + 10, " ", 1);
    memcpy(_buffer + 11, (char*)_words[2].c_str(), 8);
    memcpy(_buffer + 19, "\n", 1);
    _buffer[20] = '\0';
    cout << "Eu enviei: " << _buffer; // serve para checkar o que se enviou

    int n = udpBufferProtocol(20, 128);

    cout << "Eu recebi mas n checkei: " << _buffer; // serve para checkar o q se recebeu

    if (!checkServerAnswer(n, _buffer, "RLI ")) {
        cout << "O servidor deu a resposta errada!\n";
        return -1;
    }

    if (checkServerAnswer(n - 4, _buffer + 4, "OK\n")) {
        if (n - 7 != 0) {
            cout << "O servidor deu a resposta errada!\n";
            return -1;
        }
        cout << "Login feito ;)\n";
        return 0;
    } else if (checkServerAnswer(n - 4, _buffer + 4, "REG\n")) {
        if (n - 8 != 0) {
            cout << "O servidor deu a resposta errada!\n";
            return -1;
        }
        cout << "registro feito ;)\n";
        return 0;
    } else if (checkServerAnswer(n - 4, _buffer + 4, "NOK\n")) {
        if (n - 8 != 0) {
            cout << "O servidor deu a resposta errada!\n";
            return -1;
        }
        // password incorreta
        return 1;
    }

    cout << "O servidor deu a resposta errada!\n";

    return -1;
}

int Interface::logout() {
    memcpy(_buffer, "LOU", 3);
    memcpy(_buffer + 3, " ", 1);
    memcpy(_buffer + 4, (char*)_client->_UID.c_str(), 6);
    memcpy(_buffer + 10, " ", 1);
    memcpy(_buffer + 11, (char*)_client->_password.c_str(), 8);
    memcpy(_buffer + 19, "\n", 1);
    _buffer[20] = '\0';
    cout << "Eu enviei: " << _buffer; // serve para ter acerteza do que foi enviado

    int n = udpBufferProtocol(20, 128);

    cout << "Eu recebi mas n checkei: " << _buffer; // server para checkar o input

    if (!checkServerAnswer(n, _buffer, "RLO ")) {
        cout << "O servidor deu a resposta errada!\n";
        return -1;
    }

    if (checkServerAnswer(n - 4, _buffer + 4, "OK\n")) {
        if (n - 7 != 0) {
            cout << "O servidor deu a resposta errada!\n";
            return -1;
        }
        cout << "Logout feito ;)\n";
        delete _client;
        _client = NULL;
        return 0;
    } else if (checkServerAnswer(n - 4, _buffer + 4, "UNR\n")) {
        if (n - 8 != 0) {
            cout << "O servidor deu a resposta errada!\n";
            return -1;
        }
        cout << "User n resgistrado\n";
        return 2;
    } else if (checkServerAnswer(n - 4, _buffer + 4, "NOK\n")) {
        if (n - 8 != 0) {
            cout << "O servidor deu a resposta errada!\n";
            return -1;
        }
        cout << "User not logged in on server!\n";
        return 1;
    }

    cout << "O servidor deu a resposta errada!\n";

    return -1;
}

int Interface::unregister() {
    memcpy(_buffer, "UNR", 3);
    memcpy(_buffer + 3, " ", 1);
    memcpy(_buffer + 4, (char*)_client->_UID.c_str(), 6);
    memcpy(_buffer + 10, " ", 1);
    memcpy(_buffer + 11, (char*)_client->_password.c_str(), 8);
    memcpy(_buffer + 19, "\n", 1);
    _buffer[20] = '\0';
    cout << "Eu enviei: " << _buffer; // serve para ter acerteza do que foi enviado

    int n = udpBufferProtocol(20, 128);

    cout << "Eu recebi mas n checkei: " << _buffer; // serve para checkar o q se recebeu

    if (!checkServerAnswer(n, _buffer, "RUR ")) {
        cout << "O servidor deu a resposta errada!\n";
        return -1;
    }

    if (checkServerAnswer(n - 4, _buffer + 4, "OK\n")) {
        if (n - 7 != 0) {
            cout << "O servidor deu a resposta errada!\n";
            return -1;
        }
        cout << "Unregistered e logout feito ;)\n";
        delete _client;
        _client = NULL;
        return 0;
    } else if (checkServerAnswer(n - 4, _buffer + 4, "UNR\n")) {
        if (n - 8 != 0) {
            cout << "O servidor deu a resposta errada!\n";
            return -1;
        }
        cout << "utilizador não registado ;)\n";
        return 1;
    } else if (checkServerAnswer(n - 4, _buffer + 4, "NOK\n")) {
        if (n - 8 != 0) {
            cout << "O servidor deu a resposta errada!\n";
            return -1;
        }
        cout << "sem utilizador logged in.\n";
        return 2;
    }

    cout << "O servidor deu a resposta errada!\n";

    return -1;
}

// to modify later
int Interface::myauctions() {
    memcpy(_buffer, "LMA", 3);
    memcpy(_buffer + 3, " ", 1);
    memcpy(_buffer + 4, (char*)_client->_UID.c_str(), 6);
    memcpy(_buffer + 10, "\n", 1);
    _buffer[11] = '\0';
    cout << "Eu enviei: " << _buffer; // serve para ter acerteza do que foi enviado

    int n = udpBufferProtocol(11, 8 * 1024);

    cout << "Eu recebi mas n checkei: " << _buffer; // serve para checkar o q se recebeu

    if (!checkServerAnswer(n, _buffer, "RMA ")) {
        cout << "O servidor deu a resposta errada!\n";
        return -1;
    }

    if (checkServerAnswer(n - 4, _buffer + 4, "OK ")) {
        // fazer a listagem das auctions
        cout << "Faremos agora a listagem de suas auctions\n";
        return 0;
    } else if (checkServerAnswer(n - 4, _buffer + 4, "NLG\n")) {
        if (n - 8 != 0) {
            cout << "O servidor deu a resposta errada!\n";
            return -1;
        }
        cout << "utilizador não esta logado ;)\n";
        return 1;
    } else if (checkServerAnswer(n - 4, _buffer + 4, "NOK\n")) {
        if (n - 8 != 0) {
            cout << "O servidor deu a resposta errada!\n";
            return -1;
        }
        cout << "Não tem nenhuma ongoing auction.\n";
        return 2;
    }

    cout << "O servidor deu a resposta errada!\n";

    return -1;
}

int Interface::list() {
    memcpy(_buffer, "LST", 3);
    memcpy(_buffer + 3, "\n", 1);
    _buffer[4] = '\0';
    cout << "Eu enviei: " << _buffer; // serve para checkar o que se enviou

    int n = udpBufferProtocol(4, 8 * 1024);

    cout << "Eu recebi mas n checkei: " << _buffer; // serve para checkar o q se recebeu

    if (!checkServerAnswer(n, _buffer, "RLS ")) {
        cout << "O servidor deu a resposta errada!\n";
        return -1;
    }

    if (checkServerAnswer(n - 4, _buffer + 4, "OK ")) {
        cout << "Buffer recebido do AS.\n";
        cout << (_buffer + 7); // checkar o output!
        if (7 + 5 > n) {
            cout << "Lista de Auctions mal formatada pelo servidor!\n";
            return 0;
        }
        for (int i = 7; i < n; i += 6) {
            if (i + 5 > n) {
                cout << "Lista de Auctions mal formatada pelo servidor!\n";
                break;
            }
            char number[4];
            number[0] = _buffer[i];
            number[1] = _buffer[i + 1];
            number[2] = _buffer[i + 2];
            number[3] = '\0';
            string str = number;
            if (!isNumeric(str)) {
                cout << "Lista de Auctions mal formatada pelo servidor!\n";
                return 0;
            }
            if (_buffer[i + 3] != ' ' || (_buffer[i + 5] != ' ' && _buffer[i + 5] != '\n')) {
                cout << "Lista de Auctions mal formatada pelo servidor!\n";
                return 0;
            }
            if (_buffer[i + 4] == '0') {
                cout << "The Auction Number: " + str + " is not active!\n";
            } else if (_buffer[i + 4] == '1'){
                cout << "The Auction Number: " + str + " is active!\n";
            } else {
                cout << "Problem with auction state Number: " + str + "\n";
            }
        }
        return 0;
    } else if (checkServerAnswer(n - 4, _buffer + 4, "NOK\n")) {
        if (n - 8 != 0) {
            cout << "O servidor deu a resposta errada!\n";
            return -1;
        }
        cout << "Nenhuma auction iniciada.\n";
        return 1;
    }

    cout << "O servidor deu a resposta errada!\n";

    return -1;
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
    } else if (_words[0] == "login") {
        if (_nWords != 3) {
            cout << "Comando mal inserido: login UID(6 digits) password(8 alphanumeric)\n";
            return 0;
        }
        else {
            if (checkUIDFormat(_words[1])) {
                if (checkpasswordFormat(_words[2])) {
                    if (_client != NULL) {
                        cout << "Ja esta logged in!\n";
                        return 0;
                    }
                    int n = login();
                    if (n == -1) {
                        std::cerr << "Deu merda no login!\n";
                        return -1;
                    }
                    if (n == 0) {
                        _client = new Client(_words[1], _words[2]);
                        // cout << _client->toString(); // diz como o cliente esta
                        return 0;
                    }
                    if (n == 1) {
                        cout << "Password ou utilizador incorreta!\n";
                        return 0;
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
    } else if (_words[0] == "logout") {
        if (_nWords != 1) {
            cout << "Comando mal inserido: logout\n";
            return 0;
        } else {
            if (_client == NULL) {
                std::cout << "ja esta logged out\n";
                return 0;
            }
            int n = logout();
            if (n == -1) {
                std::cerr << "Deu merda no logout!\n";
                return -1;
            }
            return 0;
        }
    } else if (_words[0] == "unregister") {
        if (_nWords != 1) {
            cout << "Comando mal inserido: logout\n";
            return 0;
        } else {
            if (_client == NULL) {
                std::cout << "ja esta logged out\n";
                return 0;
            }
            int n = unregister();
            if (n == -1) {
                std::cerr << "Deu merda no unregister!\n";
                return -1;
            }
            /*if (n == 1) {
                cout << "User not logged in on server!\n";
                return 0;
            }
            if (n == 2) {
                cout << "User n resgistrado\n";
                return 0;
            }*/
            return 0;
        }
    } else if (_words[0] == "list" || _words[0] == "l") {
        if (_nWords != 1) {
            cout << "Comando mal inserido: list\n";
            return 0;
        } else {
            int n = list();
            if (n == -1) {
                std::cerr << "Deu merda no list!\n";
                return -1;
            }
            return 0;
        }
    } else if (_words[0] == "myauctions" || _words[0] == "ma"){
        if (_nWords != 1) {
            cout << "Comando mal inserido: myauctions\n";
            return 0;
        } else {
            if (_client == NULL) {
                cout << "Ainda não deu login!\n";
                return 0;
            }
            int n = myauctions();
            if (n == -1) {
                cerr << "Deu merda no myauctions!\n";
                return -1;
            }
            return 0;
        }
    } else {
        cout << "comando invalido!\n";
        return 0;
    }

    return 0;
}

//teste