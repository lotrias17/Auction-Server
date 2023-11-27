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
    if (argc == 1) {
        base = Interface();
        base.prepareSocket();
    }
    else {
        if (argc == 3) {
            if (strcmp(argv[1], "-p") == 0) {
                if (checkPORTFormat(argv[2])) {
                    base = Interface(stoi(argv[2]));
                } else {
                    cerr << "Formato do PORT esta errado\n";
                    exit(1);
                }
            } else {
                if (strcmp(argv[1], "-n") == 0) {
                    base = Interface(argv[2]);
                } else {
                    cerr << "flag invalida\n";
                    exit(1);
                }   
            }
        } else {
            if (argc == 5) {
                if (strcmp(argv[1], "-n") == 0 && strcmp(argv[3], "-p") == 0) {
                    if (checkPORTFormat(argv[4])) {
                        base = Interface(stoi(argv[4]), argv[2]);
                    } else {
                        cerr << "Formato do PORT esta errado\n";
                        exit(1);
                    }
                } else {
                    cerr << "flag invalida\n";
                    exit(1);
                }
            } else {
                cerr << "Numero errado de argumentos!\n";
                exit(1);
            }
        }
    }
    // fim do handle de argumentos

    while(true) {
        base.getInput();
        // cout << base.toString() << "Acabamos!\n";
        if (base.exec() == -1)
            exit(-1);
    }
}