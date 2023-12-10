#include "../asInc/database.hpp"

// User Functions ----------------------------------------------------------------

// check if uid is already registered
int checkUserRegistry(string uid) {
    struct stat filestat;
    string path = "USERS/" + uid + '/';

    int ret = stat(path.c_str(), &filestat);
    if (ret == -1) return 1;    //user never registered 1

    path += "unr.txt";
    ret = stat(path.c_str(), &filestat);
    if (ret != -1) return 2;  //user is unregistered 2

    return 0;
}

// return Client with all relevant information about UID in database
Client getUser(string uid) {
    struct stat filestat;
    FILE* fp;
    char password[9];
    int reg = checkUserRegistry(uid);

    if (reg != 0) return Client(uid, "unr");    //unregistered user

    string status = "logged in";

    string path = "USERS/" + uid + "/password.txt";
    fp = fopen(path.c_str(), "r");
    if (fp == NULL) return Client(uid, "problem");     //problema no fopen

    fread(password, sizeof(password), 1, fp);
    string str(password);

    path = "USERS/" + uid + "/login.txt";
    int ret = stat(path.c_str(), &filestat);
    if (ret == -1) status = "logged out";

    fclose(fp);

    Client res(uid, password);
    res._status = status;

    return res;
}

// add Dir, HOSTED, BIDDED, login.txt, password.txt
int addUser(string uid, string password) {
    FILE* fp;
    string userDir = "USERS/" + uid;
    string auxDir;

    //check user registry
    if (checkUserRegistry(uid) == 2) {
        unlink((userDir+"/unr.txt").c_str());
        auxDir = userDir + "/login.txt";
        fp = fopen(auxDir.c_str(), "w");
        if (fp == NULL) return -1;

        fprintf(fp, "Logged in.");
        fclose(fp);
        
        auxDir = userDir + "/password.txt";
        fp = fopen(auxDir.c_str(), "w");
        if (fp == NULL) return -1;

        fprintf(fp, "%s", password.c_str());
        fclose(fp);
        return 0;
    }

    if ((mkdir(userDir.c_str(), 0700)) == -1) return -1;    //problem making userDir

    auxDir = userDir + "/HOSTED";
    if ((mkdir(auxDir.c_str(), 0700)) == -1) {  //problem making HOSTED
        // empty userDir and then
        rmdir(userDir.c_str());
        return -1;
    }

    auxDir = userDir + "/BIDDED";
    if ((mkdir(auxDir.c_str(), 0700)) == -1) {  //problem making BIDDED
        // empty userDir and then
        rmdir(userDir.c_str());
        return -1;
    }

    auxDir = userDir + "/login.txt";
    fp = fopen(auxDir.c_str(), "w");
    if (fp == NULL) return -1;

    fprintf(fp, "Logged in.");
    fclose(fp);
    
    auxDir = userDir + "/password.txt";
    fp = fopen(auxDir.c_str(), "w");
    if (fp == NULL) return -1;

    fprintf(fp, "%s", password.c_str());
    fclose(fp);

    return 0;
}

// create login.txt
int loginUser(string uid, string password) {
    string login = "USERS/" + uid + "/login.txt";
    string path = "USERS/" + uid + "/password.txt";
    char pass[9];
    FILE* fp;

    fp = fopen(path.c_str(), "r");
    if (fp == NULL) return -1;     //problema no fopen
    fread(pass, sizeof(pass), 1, fp);
    string str(pass);

    //check if password matches
    if (pass == password) {
        fp = fopen(login.c_str(), "w");
        if (fp == NULL) return -1;
        fprintf(fp, "Logged in.");
        fclose(fp);
        return 0;
    } 
    
    return 1;
}

// delete login.txt
int logoutUser(string uid) {
    string login = "USERS/" + uid + "/login.txt";

    if (unlink(login.c_str()) == -1) return -1;

    return 0;
}

// delete login AND password, keep HOSTED and BIDDED, add unr.txt
int unregisterUser(string uid) {
    string userDir = "USERS/" + uid;
    string login = userDir + "/login.txt";
    string password = userDir + "/password.txt";
    string unr = userDir + "/unr.txt";
    FILE* fp;

    if (unlink(login.c_str()) == -1) return -1;
    if (unlink(password.c_str()) == -1) return -1;

    fp = fopen(unr.c_str(), "w");
    if (fp == NULL) return -1;

    fprintf(fp, "Unregistered.");
    fclose(fp);

    return 0;
}

int setUser(Client c) {
    if (c._status == "unregistered") {
        return unregisterUser(c._UID);
    } else if (c._status == "logged in") {
        return loginUser(c._UID, c._password);
    } else if (c._status == "logged out") {
        return logoutUser(c._UID);
    } else if (c._status == "to register") {
        return addUser(c._UID, c._password);
    }
    return 0;
}

// Auction Functions ------------------------------------------------------------------




// Bid Functions ----------------------------------------------------------------------