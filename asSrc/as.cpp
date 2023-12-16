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
    //cout << "INPUT: " << str << ": " << str.size()  << " \n";
    if (format == "uid") return str.size() == 6 && isNumeric(str); 
    else if (format == "password") return str.size() == 8 && isAlphaNumeric(str);
    else if (format == "aid") return str.size() == 3 && isNumeric(str);
    else if (format == "bid") return str.size() <= 6 && isNumeric(str);
    else return false;
}

//UID, type of request, IP, Port
void verboseOut(vector<string> input, string protocol) {    //output quando em modo verbose
    char *ip;
    int prt;
    cout << "\n-------------Received-------------\n";
    if (protocol == "udp") {       //UDP request
        if (input[0] == "LIN") cout << "Login, UID: " << input[1] << '\n';
        else if (input[0] == "LOU") cout << "Logout, UID: " << input[1] << '\n';
        else if (input[0] == "UNR") cout << "Unregister, UID: " << input[1] << '\n';
        else if (input[0] == "LMA") cout << "List Auctions, UID: " << input[1] << '\n';
        else if (input[0] == "LMB") cout << "List Bids, UID: " << input[1] << '\n';
        else if (input[0] == "LST") cout << "List All Auctions\n";
        else if (input[0] == "SRC") cout << "Show Record, AID: " << input[1] << '\n';
        else cout << "Unknown Command.\n";

        ip = inet_ntoa(udpAddr.sin_addr);
        prt = udpAddr.sin_port;
        printf("IP: %s *** PORT: %d\n",ip,prt);

    } else {    //TCP request
        if (input[0] == "OPA") cout << "Open auction, UID: " << input[1] << '\n';
        else if (input[0] == "CLS") cout << "Close Auction, UID: " << input[1] << '\n';
        else if (input[0] == "SAS") cout << "Show Asset, UID: " << input[1] << '\n';
        else if (input[0] == "BID") cout << "Bid, UID: " << input[1] << '\n';
        else cout << "Unknown Command.\n";
        ip = inet_ntoa(tcpAddr.sin_addr);
        prt = tcpAddr.sin_port;
        printf("IP: %s *** PORT: %d\n",ip,prt);
    }
    cout << "----------------------------------\n";
    return;
}

void setUdpSocket(char* p) {
    memset(&uhints, 0, sizeof(uhints));
    uhints.ai_family = AF_INET;
    uhints.ai_socktype = SOCK_DGRAM;
    uhints.ai_flags = AI_NUMERICSERV || AI_PASSIVE;

    udpErrcode=getaddrinfo(NULL, p, &uhints, &ures);
    if (udpErrcode != 0) {
        cerr << "Problema no getaddrinfo UDP.\n";
        exit(1);
    }   

    ufd=socket(ures->ai_family, ures->ai_socktype, ures->ai_protocol);
    if (ufd == -1) exit(1);     //problema a abrir socket

    n=bind(ufd, ures->ai_addr, ures->ai_addrlen);
    if (n == -1) {
        cerr << "Problema no bind UDP.\n";
        exit(1);
    }
    if (ures != NULL) freeaddrinfo(ures);
    return;
}

void setTcpSocket(char* p) {
    memset(&thints, 0, sizeof(thints));
    thints.ai_family = AF_INET;
    thints.ai_socktype = SOCK_STREAM;
    thints.ai_flags = AI_PASSIVE;

    tcpErrcode=getaddrinfo(NULL, p, &thints, &tres);
    if (tcpErrcode != 0) {
        cerr << "Problema no getaddrinfo TCP.\n";
        exit(1);
    }

    tfd=socket(tres->ai_family, tres->ai_socktype, tres->ai_protocol);
    if (tfd == -1) exit(1);     //problema a abrir socket

    n=bind(tfd, tres->ai_addr, tres->ai_addrlen);
    if (n == -1) {
        cerr << "Problema no bind TCP.\n";
        exit(1);
    }

    if (listen(tfd, 40) == -1) exit(1);
    return;
}

void receiveRequest() {
    char buffer[128];
    struct timeval timeout;

    char p[8];
    strcpy(p, to_string(port).c_str());
    
    setUdpSocket(p);
    setTcpSocket(p);

    FD_ZERO(&inputs);       //clear input mask
    FD_SET(0, &inputs);     //add default input
    FD_SET(ufd, &inputs);   //add udp socket
    FD_SET(tfd, &inputs);   //add tcp socket

    while (1) {

        testFds = inputs;    //reload mask

        memset((void *)&timeout,0,sizeof(timeout));     //timeout
        timeout.tv_sec=2;    

        outFds = select(FD_SETSIZE, &testFds, NULL, NULL, (struct timeval*) &timeout);

        switch(outFds) {
            case 0:
                cout << "Timeout event.\n";
                break;
            case -1:
                perror("select");
                break;
            default:
                for (int i = 0; i < FD_SETSIZE; i++) {
                    if (FD_ISSET(i, &testFds)) {
                        if (i == ufd){
                            addrlen = sizeof(udpAddr);
                            memset(buffer, 0, sizeof buffer);
                            n=recvfrom(ufd,buffer,128,0,(struct sockaddr*) &udpAddr, &addrlen);
                            if (n==-1) cout << "Problema no recvfrom.\n";
                            else {
                                int m = serverResponse(buffer, "udp");
                                if (m == -1) {
                                    //cerr << "Problema ao processar request.\n";
                                    n=sendto(ufd,"ERR\n",4,0,(struct sockaddr*) &udpAddr, addrlen);
                                    if (n==-1) {
                                        cout << "Problema no sendto: ERR\n";
                                    }  
                                }
                            }
                        } else if (i == tfd) {
                            addrlen = sizeof(tcpAddr);
                            if ((newfd = accept(tfd, (struct sockaddr*) &tcpAddr, &addrlen)) == -1) perror("accept");

                            FD_SET(newfd, &inputs);   
                        } else {
                            char tmp[4];
                            // tcp_info (fd, buffer, bufPos)
                            tcpBuffer *buf = getTcpBuffer(i);

                            /* if (toWrite) {
                                write(buf->_fd, path, fsize)
                            }
                            */

                            // 1 read em vez do while 
                            if (read(buf->_fd, tmp, 3) == -1) {
                                cerr << "Erro a ler buffer TCP.\n";
                                exit(1);
                            }
                            buf->update(tmp);
                            
                            // exceção de OPA
                            if (buf->_buffer == "OPA") {
                                // check for 7(?) spaces -> addAuction
                                // start writing to file, from read (bool toWrite)
                                // write(buf->_fd,)
                            }

                            // só vai para serverResponse se apanhar \n   
                            if (buf->_buffer.find('\n') != string::npos) {
                                if (serverResponse(buf->_buffer, "tcp") == -1) {
                                    //cerr << "Problema ao processar request.\n";
                                    if (write(i, "ERR\n", 4) == -1) perror("tcp write");
                                }
                                // response is handled by specific functions: serverResponse()
                                removeTcp(i);
                                close(i);
                                FD_CLR(i, &inputs);
                            }
                            
                        }
                    }
                }
        }

    }

    freeaddrinfo(tres);
    close(tfd);
    freeaddrinfo(ures);
    close(ufd);
    return;
}

int serverResponse(string buffer, string protocol) {
    int n = 0;
    vector<string> input;
    string s;
    stringstream ss;
    ss << buffer;
    
    //cout << "Buffer:\n";
    while(getline(ss, s, ' ')) {
        input.push_back(s);
        n++;
        //cout << s << '\n';
    }
    //cout << ":\n";
    input[n-1].pop_back();

    if (verbose) verboseOut(input, protocol);

    if (input[0] == "LIN") {    //UDP
        if ((processLogin(input[1], input[2])) == -1) {
            cerr << "Problema a processar login.\n";
            return -1;
        } 
    } else if (input[0] == "LOU") {     //UDP
        if ((processLogout(input[1])) == -1) {
            cerr << "Problema a processar logout.\n";
            return -1;
        }
    } else if (input[0] == "UNR") {     //UDP
        if ((processUnregister(input[1])) == -1) {
            cerr << "Problema a processar unregister.\n";
            return -1;
        }
    } else if (input[0] == "LMA") {     //UDP
        if ((processListMyAuctions(input[1])) == -1) {
            
            cerr << "Problema a processar LMA.\n";
            return -1;
        }
    } else if (input[0] == "LMB") {     //UDP
        if ((processListMyBids(input[1])) == -1) {
            cerr << "Problema a processar LMB.\n";
            return -1;
        }
    } else if (input[0] == "LST") {     //UDP
        if ((processList()) == -1) {
            cerr << "Problema a processar LST.\n";
            return -1;
        }
    } else if (input[0] == "SRC") {     //UDP
        if ((processShowRecord(input[1])) == -1) {
            cerr << "Problema a processar SRC.\n";
            return -1;
        }
    } else if (input[0] == "OPA") {     //TCP
        if ((processOpen(input)) == -1) {
            cerr << "Problema a processar OPA.\n";
            return -1;
        }
    } else if (input[0] == "CLS") {     //TCP
        if ((processClose(input[1], input[3])) == -1) {
            cerr << "Problema a processar CLS.\n";
            return -1;
        }
    } else if (input[0] == "SAS") {     //TCP
        if ((processBid(input[1], input[3], input[4])) == -1) {
            cerr << "Problema a processar SAS.\n";
            return -1;
        }
    } else if (input[0] == "BID") {     //TCP
        if ((processBid(input[1], input[3], input[4])) == -1) {
            cerr << "Problema a processar BID.\n";
            return -1;
        }
    }
    else {
        //cerr << "Unknown Command.\n";
        return -1;
    }
    
    return 0;
}

// UDP functions ------------------------------------

int processLogin(string uid, string password) {
    int status = 0;
    // atualizar database
    // retornar estado 
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

        //cout << "Registered User: " << uid << '\n';
        //buf = "RLI REG\n";
        n=sendto(ufd,"RLI REG\n",8,0,(struct sockaddr*) &udpAddr, addrlen);
        if (n==-1) cout << "Problema no sendto: RLI REG\n";
    } else if (c._status == "logged out" || c._status == "logged in") {   //RLI OK
        c._password = password;     //atualizar user
        c._status = "logged in";
        status = setUser(c);

        if (status == -1) return -1;   //atualizar database
        else if (status == 0) {
            n=sendto(ufd,"RLI OK\n",7,0,(struct sockaddr*) &udpAddr, addrlen);
            if (n==-1) cout << "Problema no sendto: RLI OK\n";
        } else {
            //cout << "Wrong password.\n";    //RLI NOK - wrong password 
            n=sendto(ufd,"RLI NOK\n",8,0,(struct sockaddr*) &udpAddr, addrlen);
            if (n==-1) cout << "Problema no sendto: RLI NOK\n";
        }
    } else {
        cout << "Erro." << '\n';
        return -1;
    }
    // return buf;
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
        //cout << "Unregistered User: " << uid << '\n';
        n=sendto(ufd,"RLO UNR\n",8,0,(struct sockaddr*) &udpAddr, addrlen);
        if (n==-1) cout << "Problema no sendto: RLO UNR\n";
    } else if (c._status == "logged out") {   //RLO NOK
        //cout << "User already logged out: " << uid << '\n';
        n=sendto(ufd,"RLO NOK\n",8,0,(struct sockaddr*) &udpAddr, addrlen);
        if (n==-1) cout << "Problema no sendto: RLO NOK\n";
    } else if (c._status == "logged in") {    //RLO OK
        c._status = "logged out";
        if (setUser(c) == -1) return -1;
        //cout << "Logged out User: " << uid << '\n';
        n=sendto(ufd,"RLO OK\n",7,0,(struct sockaddr*) &udpAddr, addrlen);
        if (n==-1) cout << "Problema no sendto: RLO OK\n";
    } else {
        cout << "Erro." << '\n';
        return -1;
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
        //cout << "User: " << uid << "is not registered." << '\n';
        n=sendto(ufd,"RUR UNR\n",8,0,(struct sockaddr*) &udpAddr, addrlen);
        if (n==-1) cout << "Problema no sendto: RUR UNR\n";
    } else if (c._status == "logged out") {   //RUR NOK
        //cout << "User is logged out: " << uid << '\n';
            n=sendto(ufd,"RUR NOK\n",8,0,(struct sockaddr*) &udpAddr, addrlen);
            if (n==-1) cout << "Problema no sendto: RUR NOK\n";
    } else if (c._status == "logged in") {    //RUR OK
        c._status = "unregistered";
        if (setUser(c) == -1) return -1;
        //cout << "Unregistered User: " << uid << '\n';
            n=sendto(ufd,"RUR OK\n",7,0,(struct sockaddr*) &udpAddr, addrlen);
            if (n==-1) cout << "Problema no sendto: RUR OK\n";
    } else {
        cout << "Erro." << '\n';
        return -1;
    }
    return 0;
}

int processListMyAuctions(string uid) {
    if (!checkFormat("uid", uid)) {
        cerr << "Poorly formatted request.\n";
        return -1;
    }

    Client c = getUser(uid);
    if (c._password == "problem") return -1;

    //enviar ACK
    if (c._status == "unregistered" || c._status == "logged out") {  //RMA NLG
        //cout << "User: " << uid << "is not logged in." << '\n';
        n=sendto(ufd,"RMA NLG\n",8,0,(struct sockaddr*) &udpAddr, addrlen);
        if (n==-1) cout << "Problema no sendto: RMA NLG\n";
    } else if (c._status == "logged in") {    //RMA OK|NOK
        string list = listAuctions(getUserAuctions(uid, "HOSTED"));
        //cout << "LIST:" << list << ":\n";
        if (list == "") {       //RMA NOK - no HOSTED auctions
            n=sendto(ufd,"RMA NOK\n",8,0,(struct sockaddr*) &udpAddr, addrlen);
            if (n==-1) cout << "Problema no sendto: RMA NOK\n";

        } else {
            string buffer = "RMA OK";
            buffer += list + '\n';
            n=sendto(ufd,buffer.c_str(),buffer.length(),0,(struct sockaddr*) &udpAddr, addrlen);
            if (n==-1) cout << "Problema no sendto: RMA OK\n";
        }
    } else {
        cout << "Erro." << '\n';
        return -1;
    }
    return 0;
}

int processListMyBids(string uid) {
    if (!checkFormat("uid", uid)) {
        cerr << "Poorly formatted request.\n";
        return -1;
    }

    Client c = getUser(uid);
    if (c._password == "problem") return -1;

    //enviar ACK
    if (c._status == "unregistered" || c._status == "logged out") {  //RMB NLG
        //cout << "User: " << uid << "is not logged in." << '\n';
        n=sendto(ufd,"RMB NLG\n",8,0,(struct sockaddr*) &udpAddr, addrlen);
        if (n==-1) cout << "Problema no sendto: RMB NLG\n";
    } else if (c._status == "logged in") {    //RMA OK|NOK
        string list = listAuctions(getUserAuctions(uid, "BIDDED"));
        //cout << "LIST:" << list << ":\n";
        if (list == "") {       //RMB NOK - no BIDDED bids
            n=sendto(ufd,"RMB NOK\n",8,0,(struct sockaddr*) &udpAddr, addrlen);
            if (n==-1) cout << "Problema no sendto: RMB NOK\n";

        } else {
            string buffer = "RMB OK";
            buffer += list + '\n';
            n=sendto(ufd,buffer.c_str(),buffer.length(),0,(struct sockaddr*) &udpAddr, addrlen);
            if (n==-1) cout << "Problema no sendto: RMB OK\n";
        }
    } else {
        cout << "Erro." << '\n';
        return -1;
    }
    return 0;
}

int processList() {

    string list = listAuctions(getAllAuctions());
    //cout << "LIST:" << list << ":\n";
    if (list == "") {       //RLS NOK - no Auctions yet
        n=sendto(ufd,"RLS NOK\n",8,0,(struct sockaddr*) &udpAddr, addrlen);
        if (n==-1) cout << "Problema no sendto: RLS NOK\n";
    } else {
        string buffer = "RLS OK";
        buffer += list + '\n';
        n=sendto(ufd,buffer.c_str(),buffer.length(),0,(struct sockaddr*) &udpAddr, addrlen);
        if (n==-1) cout << "Problema no sendto: RMB OK\n";
    }
    return 0;
}

int processShowRecord(string aid) {
    if (!checkFormat("aid", aid)) {
        cerr << "Poorly formatted request.\n";
        return -1;
    }

    string ans = "RRC NOK\n";

    // Check if aid exists (RRC NOK)
    Auction a = getAuction(aid);
    if (a._aid == "!") {
        n=sendto(ufd,ans.c_str(),8,0,(struct sockaddr*) &udpAddr, addrlen);
        if (n==-1) cout << "Problema no sendto: RRC NOK\n";
        return 1;
    }

    //  (RRC OK)
    ans = "RRC OK ";
    ans += showAuctionRecord(aid);
    ans += "\n";

    cout << "SENT:" << ans << ":\n";

    n=sendto(ufd,ans.c_str(),ans.size(),0,(struct sockaddr*) &udpAddr, addrlen);
    if (n==-1) cout << "Problema no sendto: RRC OK\n";
    return 1;
}

// TCP functions ------------------------------------

int processOpen(vector<string> input) {         // CHECK FNAME AND FSIZE!!!!!!!!!!!!!!!!!!!
    string uid = input[1];
    string password = input[2];
    string name = input[3];
    string startValue = input[4];
    string timeActive = input[5];
    string Fname = input[6];
    string Fsize = input[7];
    string Fdata = input[8];

    //check uid, password
    if (!checkFormat("uid", uid) || !checkFormat("password", password)) {
        cerr << "Poorly formatted request.\n";
        return -1;
    }
    //check name, startValue, timeactive
    if (!isAlphaNumeric(name) || name.length() > 10) {
        //cout << "O nome do produto tem de ser menor que 10 characteres!\n";
        cerr << "Poorly formatted request.\n";
        return -1;
    } else if (!isNumeric(startValue) || startValue.length() > 6) {
        //cout << "O start value tem que ser um numero inferior a 10^7!\n";
        cerr << "Poorly formatted request.\n";
        return -1;
    } else if (!isNumeric(timeActive) || timeActive.length() > 5) {
        //cout << "O time dever ser um numero inferor a 10^6!\n";
        cerr << "Poorly formatted request.\n";
        return -1;
    }

    //check if user is logged in
    Client c = getUser(uid);
    if (c._password == "problem") return -1;

    if (c._status != "logged in") {
        //ROA NLG if uid is not logged in
        n=write(newfd,"ROA NLG\n", 8);
        if (n==-1) cout << "Problema no sendto: ROA NLG\n";
        return 0;
    }

    string aid = addAuction(input);
    //create auction, update database
    if (aid == "!") {      // problema a adicionar Auction
        n=write(newfd,"ROA NOK\n", 8);   // ROA NOK
        if (n==-1) cout << "Problema no sendto: ROA NOK\n";
    } else {
        string ans = "ROA OK " + aid + "\n";
        n=write(newfd, ans.c_str(), 11);   // ROA OK
        if (n==-1) cout << "Problema no sendto: ROA OK\n";
    }

    return 0;
}

int processClose(string uid, string aid) { 
    if (!checkFormat("uid", uid)) {
        cerr << "Poorly formatted request.\n";
        return -1;
    }

    //check if uid is logged in (RCL NLG)
    Client c = getUser(uid);
    if (c._password == "problem") return -1;

    if (c._status != "logged in") {
        n=write(newfd,"RCL NLG\n", 8);   // RCL NLG
        if (n==-1) cout << "Problema no sendto: RCL NLG\n";
        return 1;
    }

    //check if aid exists   (RCL EAU)
    Auction a = getAuction(aid);
    if (a._aid != aid) {        // nao existe auction (aid)
        n=write(newfd,"RCL EAU\n", 8);   // RCL EAU
        if (n==-1) cout << "Problema no sendto: RCL EAU\n";
        return 1;
    }

    //check if aid was HOSTED by uid (RCL EOW)
    if (to_string(a._uid) != uid) {
        n=write(newfd,"RCL EOW\n", 8);   // RCL EOW
        if (n==-1) cout << "Problema no sendto: RCL EOW\n";
        return 1;
    }

    if (!endAuction(aid)) {
        n=write(newfd,"RCL END\n", 8);   // RCL END
        if (n==-1) cout << "Problema no sendto: RCL END\n";
    } else {
        // RCL OK, end auction by adding END_aid.txt to AUCTIONS/aid/
        n=write(newfd,"RCL OK\n", 7);   // RCL OK
        if (n==-1) cout << "Problema no sendto: RCL OK\n";
    }
    return 0;
}

int processBid(string uid, string aid, string bid) {
    if (!checkFormat("uid", uid) || !checkFormat("aid", aid) || !checkFormat("bid", bid)) {
        cerr << "Poorly formatted request.\n";
        return -1;
    }
    
    Client c = getUser(uid);
    if (c._password == "problem") return -1;

    Auction a = getAuction(aid);

    //check if uid is logged in (RBD NLG)
    if (c._status != "logged in") {
        n=write(newfd,"RBD NLG\n", 8);   // RBD NLG
        if (n==-1) cout << "Problema no sendto: RBD NLG\n";
        return 1;
    }

    //check if aid exists and is active (RBD NOK)
    if (a._state == 0 || a._aid == "!") {
        n=write(newfd,"RBD NOK\n", 8);   // RBD NOK
        if (n==-1) cout << "Problema no sendto: RBD NOK\n";
        return 1;
    }

    //check if aid belongs to uid (RBD ILG)
    if (a._uid == stoi(uid)) {
        n=write(newfd,"RBD ILG\n", 8);   // RBD ILG
        if (n==-1) cout << "Problema no sendto: RBD ILG\n";
        return 1;
    }

    //check if bid is higher than previous highest bid (RBD REF)
    if (stoi(bid) <= a._highValue) {
        n=write(newfd,"RBD REF\n", 8);   // RBD REF
        if (n==-1) cout << "Problema no sendto: RBD REF\n";
    } else {    // RBD ACC, aid._highValue = bid;
        addBid(uid, aid, stoi(bid));
        n=write(newfd,"RBD ACC\n", 8);   // RBD ACC
        if (n==-1) cout << "Problema no sendto: RBD ACC\n";
    }
    return 1;
}

//int processShowAsset() {}

int main(int argc, char** argv) {
    cout << "Ola meu servidor!\n";

    // handle arguments

    bool args[2];
    int portNumber;
    string check;
    args[0] = false; // isto e o arg p
    args[1] = false; // isto e o arg v
    
    if (argc != 1 && argc != 2 && argc != 4) {
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
}