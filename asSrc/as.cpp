#include "../asInc/AS.hpp"

using namespace std;

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

void receiveRequest() {
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

        cout << "Y\n";

        addrlen = sizeof(addr);
        n=recvfrom(fd,buffer,128,0,(struct sockaddr*) &addr, &addrlen);
        if (n==-1) {
            cout << "Problema no recvfrom.\n";
        }
        else {
            int m = serverResponse(buffer);
            if (m == -1) cerr << "Problema ao processar request.\n";
            else if (m == 0) break;
        }

        write(1, "received: ", 10); write(1, buffer, n);
        n=sendto(fd,buffer,n,0,(struct sockaddr*) &addr, addrlen);
        if (n==-1) {
            cout << "Problema no sendto.\n";
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
        }
    } else if (input[0] == "LOU") {
        cout << "Process Logout:\n";
    } else if (input[0] == "UNR") {
        cout << "Process Unregister:\n";
    } else if (input[0] == "LST") {
        cout << "Process List:\n";
    }

    //cout << strlen(buf) << ": " << buf;

    return 1;
}

int processLogin(string uid, string password) {

    if (!checkFormat("uid", uid) || !checkFormat("password", password)) {
        cerr << "Poorly formatted request.\n";
        return -1;
    }

    auto res = userList.find(uid);

    cout << "ASH.\n";

    if (res == userList.end()) {  //unregistered
        Client* c = new Client(uid, password);
        userList.insert({uid, c});
        cout << "Registered User: " << uid << '\n';
        return 0;
    }

    return 0;
}

bool checkFormat(string format, string str) {
    if (format == "uid") return str.size() == 6 && isNumeric(str); 
    else if (format == "password") return str.size() == 8 && isAlphaNumeric(str);
    else return false;
}




int main() {
    userList.erase(userList.begin(), userList.end());
    cout << "Ola meu servidor!\n";

    receiveRequest();

    // receive user request
    // process request
    // send answer back (either simple or verbose, TCP or UDP)


    freeaddrinfo(res);
    close(fd);
}