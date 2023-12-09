#include "../asInc/AS.hpp"

bool checkPORTFormat(char* str) {
    for(int i = 0; i < 5; i++) {
        if (!isdigit(str[i]))
            return false;
    }
    return stoi(str) < 65536;
}

bool isNumeric(string str) {
    int size = (int) str.size();
    for (int i = 0; i < size; i++) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}

bool isAlphaNumeric(string str) {
    int size = (int) str.size();    
    for (int i = 0; i < size; i++) {
        if (!isalnum(str[i])) {
            return false;
        }
    }
    return true;
}

bool checkFormat(string format, string str) {
    if (format == "uid") return str.size() == 6 && isNumeric(str); 
    else if (format == "password") return str.size() == 8 && isAlphaNumeric(str);
    else return false;
}

//string verboseOut() {}    // o que por no output quando em modo verbose

void receiveRequest() {
    fd=socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) {
        cerr << "Problema no socket UDP.\n";
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    char p[8];
    strcpy(p, to_string(port).c_str());

    errcode=getaddrinfo(NULL, p, &hints, &res);
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

        cout << "Y\n";

        addrlen = sizeof(addr);
        n=recvfrom(fd,buffer,128,0,(struct sockaddr*) &addr, &addrlen);
        if (n==-1) {
            cout << "Problema no recvfrom.\n";
        }
        else {
            int m = serverResponse(buffer);
            if (m == -1) {
                cerr << "Problema ao processar request.\n";
                n=sendto(fd,"ERR\n",4,0,(struct sockaddr*) &addr, addrlen);
                if (n==-1) {
                    cout << "Problema no sendto: ERR\n";
                }  
            }
            else if (m == 0) break;
        }
    }
    return;
}

int serverResponse(char* buffer) {
    int n;
    vector<string> input;
    string s;
    stringstream buf;
    buf << buffer;

    while(getline(buf, s, ' ')) {
        input.push_back(s);
    }
    input[2].pop_back();
 
    if (input[0] == "LIN") {
        cout << "Process Login:\n";
        if ((n = processLogin(input[1], input[2])) == -1) {
            cerr << "Problema a processar login.\n";
            return -1;
        } 
    } else if (input[0] == "LOU") {
        cout << "Process Logout:\n";
        if ((n = processLogout(input[1])) == -1) {
            cerr << "Problema a processar logout.\n";
            return -1;;
        }
    } else if (input[0] == "UNR") {
        cout << "Process Unregister:\n";
        if ((n = processUnregister(input[1])) == -1) {
            cerr << "Problema a processar logout.\n";
            return -1;;
        }
    } else if (input[0] == "LST") {
        cout << "Process List:\n";
    }

    //cout << strlen(buf) << ": " << buf;

    return 1;
}

int processLogin(string uid, string password) {
    int status = 0;

    if (!checkFormat("uid", uid) || !checkFormat("password", password)) {
        cerr << "Poorly formatted request.\n";
        return -1;
    }

    Client c = getUser(uid);
    if (c._password == "problem") return -1;

    //enviar ACK
    if (c._status == "unregistered") {  //RLI REG
        c._password = password;     //atualizar user
        c._status = "to register";
        if (setUser(c) == -1) return -1;   //atualizar database

        cout << "Registered User: " << uid << '\n';
        n=sendto(fd,"RLI REG\n",8,0,(struct sockaddr*) &addr, addrlen);
        if (n==-1) cout << "Problema no sendto: RLI REG\n";
    } else if (c._status == "logged out" || c._status == "logged in") {   //RLI OK
        c._password = password;     //atualizar user
        c._status = "logged in";
        status = setUser(c);

        if (status == -1) return -1;   //atualizar database
        else if (status == 0) {
            cout << "Logged in User: " << uid << '\n';
            n=sendto(fd,"RLI OK\n",7,0,(struct sockaddr*) &addr, addrlen);
            if (n==-1) cout << "Problema no sendto: RLI OK\n";
        } else {
            cout << "Wrong password.\n";    //RLI NOK - wrong password 
            n=sendto(fd,"RLI NOK\n",8,0,(struct sockaddr*) &addr, addrlen);
            if (n==-1) cout << "Problema no sendto: RLI NOK\n";
        }
    } else {
        cout << "Erro." << '\n';
        n=sendto(fd,"ERR\n",4,0,(struct sockaddr*) &addr, addrlen);
        if (n==-1) cout << "Problema no sendto: login\n";
    }
    
    return 0;
}

int processLogout(string uid) {
    if (!checkFormat("uid", uid)) {
        cerr << "Poorly formatted request.\n";
        return -1;
    }

    Client c = getUser(uid);
    if (c._password == "problem") return -1;

    //enviar ACK
    if (c._status == "unregistered") {  //RLO UNR
        cout << "Unregistered User: " << uid << '\n';
        n=sendto(fd,"RLO UNR\n",8,0,(struct sockaddr*) &addr, addrlen);
        if (n==-1) cout << "Problema no sendto: RLO UNR\n";
    } else if (c._status == "logged out") {   //RLO NOK
        cout << "User already logged out: " << uid << '\n';
        n=sendto(fd,"RLO NOK\n",8,0,(struct sockaddr*) &addr, addrlen);
        if (n==-1) cout << "Problema no sendto: RLO NOK\n";
    } else if (c._status == "logged in") {    //RLO OK
        c._status = "logged out";
        if (setUser(c) == -1) return -1;
        cout << "Logged out User: " << uid << '\n';
        n=sendto(fd,"RLO OK\n",7,0,(struct sockaddr*) &addr, addrlen);
        if (n==-1) cout << "Problema no sendto: RLI OK\n";
    } else {
        cout << "Erro." << '\n';
        n=sendto(fd,"ERR\n",4,0,(struct sockaddr*) &addr, addrlen);
        if (n==-1) cout << "Problema no sendto: login\n";
    }
    return 0;
}

int processUnregister(string uid) {

     if (!checkFormat("uid", uid)) {
        cerr << "Poorly formatted request.\n";
        return -1;
    }

    Client c = getUser(uid);
    if (c._password == "problem") return -1;

    //enviar ACK
    if (c._status == "unregistered") {  //RUR UNR
        cout << "User: " << uid << "is not registered." << '\n';
        n=sendto(fd,"RUR UNR\n",8,0,(struct sockaddr*) &addr, addrlen);
        if (n==-1) cout << "Problema no sendto: RUR UNR\n";
    } else if (c._status == "logged out") {   //RUR NOK
        cout << "User is logged out: " << uid << '\n';
            n=sendto(fd,"RUR NOK\n",8,0,(struct sockaddr*) &addr, addrlen);
            if (n==-1) cout << "Problema no sendto: RUR NOK\n";
    } else if (c._status == "logged in") {    //RUR OK
        c._status = "unregistered";
        if (setUser(c) == -1) return -1;
        cout << "Unregistered User: " << uid << '\n';
            n=sendto(fd,"RUR OK\n",7,0,(struct sockaddr*) &addr, addrlen);
            if (n==-1) cout << "Problema no sendto: RLI OK\n";
    } else {
        cout << "Erro." << '\n';
        n=sendto(fd,"ERR\n",4,0,(struct sockaddr*) &addr, addrlen);
        if (n==-1) cout << "Problema no sendto: login\n";
    }
    return 0;
}

int main(int argc, char** argv) {
    cout << "Ola meu servidor!\n";

    // handle arguments

    bool args[2];
    int portNumber;
    string check;
    args[0] = false; // isto e o arg p
    args[1] = false; // isto e o arg v
    
    if (argc != 1 && argc != 3 && argc != 4) {
        cerr << "Numero errado de argumentos!\n";
        exit(1);
    }

    // loop to get every arg
    for (int i = 1; i < argc; i++) {
        check = argv[i];
        //cout << "CHECK: " << check << '\n';
        if (check == "-p") {
            if (args[0] == true) {
                cout << "Argumento repetido!\n";
                exit(1);
            } else {
                if (checkPORTFormat(argv[i + 1])) {
                    portNumber = stoi(argv[i + 1]);
                    args[0] = true;
                    i++;
                } else {
                    cerr << "Formato do PORT esta errado\n";
                    exit(1);
                }
            }
        } else if (check == "-v") {
            if (args[1] == true) {
                cout << "Argumento repetido!\n";
                exit(1);
            } else {
                args[1] = true;
            }
        } else {
            cerr << "flag invalida\n";
            exit(1);
        }
    }

    if (args[1]) verbose = true;
    if (args[0]) port = portNumber;

    receiveRequest();

    // receive user request
    // process request
    // send answer back (either simple or verbose, TCP or UDP)


    freeaddrinfo(res);
    close(fd);
}