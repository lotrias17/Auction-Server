#include "../userInc/user.hpp"

using namespace std;

bool checkPORTFormat(char* str) {
    for(int i = 0; i < 5; i++) {
        if (!isdigit(str[i]))
            return false;
    }
    return stoi(str) < 65536;
}

int main(int argc, char** argv) {
    Interface base;
    // handle de argumentos

    bool args[2];
    int strs = 0;
    int stri;
    string check;
    args[0] = false; // isto e o arg n
    args[1] = false; // isto e o arg p
    
    if (argc % 2 != 1) {
        cerr << "Numero errado de argumentos!\n";
        exit(1);
    }

    // loop to get every arg
    for (int i = 1; i < argc; i += 2) {
        check = argv[i];
        if (check == "-n") {
            // bla bla bla n
            if (args[0] == true) {
                cout << "Argumento repetido!\n";
                exit(1);
            } else {
                strs = i + 1;
                args[0] = true;
            }
        } else if (check == "-p") {
            // bla bla bla p
            if (args[1] == true) {
                cout << "Argumento repetido!\n";
                exit(1);
            } else {
                if (checkPORTFormat(argv[i + 1])) {
                    stri = stoi(argv[i + 1]);
                    args[1] = true;
                } else {
                    cerr << "Formato do PORT esta errado\n";
                    exit(1);
                }
            }
        } else {
            cerr << "flag invalida\n";
            exit(1);
        }
    }

    if (args[0] && args[1]) {
        base = Interface(stri, argv[strs]);
        cout << base.toString();
    } else if (args[0]) {
        base = Interface(argv[strs]);
        cout << base.toString();
    } else if (args[1]) { 
        base = Interface(stri);
        cout << base.toString();
    }
    base.prepareSocket();

    // fim do handle de argumentos

    int n = 0;

    while(true) {
        n = base.getInput();
        //cout << base.toString() ;
        //cout << "Acabamos!\n";
        if (n == -1) {
            continue;
        }
        if (base.exec() == -1)
            exit(-1);
    }
    
}