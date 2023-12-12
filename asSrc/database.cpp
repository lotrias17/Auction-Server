#include "../asInc/database.hpp"

int currAid = 0;

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

vector<Auction> getUserAuctions(string uid, string dir) {
    vector<Auction> auctionList;
    FILE* fp;
    char state[1];
    struct dirent **filelist;
    int numEntries;
    string usedDir = "USERS/" + uid + '/' + dir + '/';
    string name, path;
    
    
    numEntries = scandir(usedDir.c_str(), &filelist, 0, alphasort);
    if (numEntries <= 2) {
        auctionList.push_back(Auction("-1"));     //user has no auctions
        return auctionList;
    }
    while (numEntries--) {
        name = filelist[numEntries]->d_name;
        if (name.length() == 7) {
            path = usedDir + name;

            string aid = name.substr(0, name.find_last_of("."));

            //read file for state
            fp = fopen(path.c_str(), "r");
            if (fp == NULL) {       //problema no fopen
                auctionList.erase(auctionList.begin(), auctionList.end());
                return auctionList;     
            } 
            fread(state, sizeof(state), 1, fp);

            auctionList.push_back(Auction(stoi(uid), aid, atoi(state)));
        }
        free(filelist[numEntries]);
    }
    free(filelist);
    return auctionList;
}

vector<Auction> getAllAuctions() {      //through HOSTED in every client
    vector<Auction> auctionList, auxList;
    string uid, userDir = "USERS/";
    struct dirent **filelist;
    int numEntries;

    numEntries = scandir(userDir.c_str(), &filelist, 0, alphasort);

    cout << "NUM:" << to_string(numEntries) << '\n';

    if (numEntries <= 2) {
        auctionList.push_back(Auction("-1"));     //there are no clients
        return auctionList;
    }
    while (numEntries--) {
        uid = filelist[numEntries]->d_name;
        if (uid.length() == 6) {
            auxList = getUserAuctions(uid, "HOSTED");
            auctionList.insert(auctionList.end(), auxList.begin(), auxList.end());
        }
    }
    return auctionList;
}

string listAuctions(vector<Auction> list) {
    string res = "";
    int size = list.size();
    for (int i = 0; i<size; i++) {
        if (stoi(list[i]._aid) == -1) {
            break;
        }
        res += " " + list[i].simpleToString();
    }
    return res;
}

int addAuction(vector<string> input) {
    string uid = input[1];
    string password = input[2];
    string name = input[3];
    string startValue = input[4];
    string timeActive = input[5];
    string Fname = input[6];
    string Fsize = input[7];
    string Fdata = input[8];
    FILE* fp;
    time_t fullTime;
    struct tm* date;

    Auction auc = Auction(stoi(uid), name, stoi(startValue), stoi(timeActive));
    if (currAid < 1000) auc._aid = ++currAid;
    else return -1;

    string aucDir = "AUCTIONS/" + aidToString(currAid) + "/";
    // criar diretoria (aid)/
    if ((mkdir(aucDir.c_str(), 0700)) == -1) return -1;    //problema a fazer aucDir

    // criar diretoria BIDS/ e diretoria ASSET/
    string path = aucDir + "BIDS/";
    if ((mkdir(path.c_str(), 0700)) == -1) return -1;    //problema a fazer BIDS/

    // criar START_(aid).txt com UID name Fname startValue timeactive startDateTime startFullTime
    time(&fullTime);
    date = gmtime(&fullTime);
    
    string s = uid + " " + name + " " + Fname + " " + startValue + " " + timeActive + " "
    + timeToString(date) + " " + to_string(fullTime);

    path = aucDir + "START_" + to_string(currAid) + ".txt";
    fp = fopen(path.c_str(), "w");
    if (fp == NULL) return -1;
    fwrite(Fdata.c_str(), sizeof(char), stoi(Fsize), fp);
    fclose(fp);

    path = aucDir + "ASSET/";
    if ((mkdir(path.c_str(), 0700)) == -1) return -1;    //problem making userDir

    path += Fname;
    
    // adicionar asset com Fname, Fsize e Fdata à diretoria ASSET/
    cout << "Vou tentar escrever!\n";
    ofstream file;

    file.open(path.c_str());

    file << Fdata;

    file.close();

    return currAid;
}

string timeToString(tm* tm) {
    char t[20];
    sprintf(t, "%4d-%02d-%02d %02d:%02d:%02d", tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
    tm->tm_hour, tm->tm_min, tm->tm_sec);
    string s = t;
    return s;
}

string aidToString(int aid) {
    string id = to_string(aid);
    if (id.length() == 3) return id;
    else if (id.length() == 2) return "0" + id;
    else if (id.length() == 1) return "00" + id;
    else return "!"; 
}

// Bid Functions ----------------------------------------------------------------------