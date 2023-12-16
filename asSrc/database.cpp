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
    struct dirent **filelist;
    int numEntries;
    string usedDir = "USERS/" + uid + '/' + dir + '/';
    string aid, name, path;
    
    
    numEntries = scandir(usedDir.c_str(), &filelist, 0, alphasort);
    if (numEntries <= 2) {
        auctionList.push_back(Auction("-1"));     //user has no auctions
        return auctionList;
    }
    while (numEntries--) {
        name = filelist[numEntries]->d_name;
        if (name.length() == 7) {
            path = usedDir + name;

            aid = name.substr(0, name.find_last_of("."));
            Auction a = getAuction(aid);
            auctionList.push_back(a);
        }
        free(filelist[numEntries]);
    }
    free(filelist);
    return auctionList;
}

vector<Auction> getAllAuctions() {
    vector<Auction> auctionList, auxList;
    string aid, aucDir = "AUCTIONS/";
    struct dirent **filelist;
    int numEntries;

    numEntries = scandir(aucDir.c_str(), &filelist, 0, alphasort);

    if (numEntries <= 3) {
        cout << "NO Auctions\n";
        auctionList.push_back(Auction("-1"));     //there are no auctions
        return auctionList;
    }
    while (numEntries--) {
        aid = filelist[numEntries]->d_name;
        if (aid.length() == 3) {
            Auction tmp = getAuction(aid);
            auctionList.push_back(tmp);
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

string addAuction(vector<string> input) {
    string uid = input[1];
    string password = input[2];
    string name = input[3];
    string startValue = input[4];
    string timeActive = input[5];
    string Fname = input[6];
    string Fsize = input[7];
    string Fdata = input[8];
    time_t fullTime;
    struct tm* date;
    int numEntries;
    struct dirent **filelist;
    ofstream file;

    // determine currAid: numEntries - 3 (. .. and .gitignore)
    numEntries = scandir("AUCTIONS/", &filelist, 0, alphasort);
    currAid = numEntries - 3;

    Auction auc = Auction(stoi(uid), name, stoi(startValue), stoi(timeActive));
    if (currAid < 1000) auc._aid = ++currAid;
    else return aidToString(-1);

    string aucDir = "AUCTIONS/" + aidToString(currAid) + "/";
    // criar diretoria (aid)/
    if ((mkdir(aucDir.c_str(), 0700)) == -1) return aidToString(-1);    //problema a fazer aucDir

    // criar diretoria BIDS/ e diretoria ASSET/
    string path = aucDir + "BIDS/";
    if ((mkdir(path.c_str(), 0700)) == -1) return aidToString(-1);    //problema a fazer BIDS/

    // criar START_(aid).txt com UID name Fname startValue timeactive startDateTime startFullTime
    time(&fullTime);
    date = gmtime(&fullTime);

    string s = uid + " " + name + " " + Fname + " " + startValue + " " + timeActive + " "
    + timeToString(date) + " " + to_string(fullTime);

    path = aucDir + "START_" + aidToString(currAid) + ".txt";

    file.open(path.c_str());
    file << s;
    file.close();

    path = aucDir + "ASSET/";
    if ((mkdir(path.c_str(), 0700)) == -1) return aidToString(-1);    //problem making userDir

    path += Fname;
    
    // adicionar asset com Fname, Fsize e Fdata à diretoria ASSET/
    file.open(path.c_str(), ios::binary);
    file << Fdata;
    file.close();

    // adicionar (aid).txt na diretoria USERS/(uid)/HOSTED/
    path = "USERS/" + uid + "/HOSTED/" + aidToString(currAid) + ".txt";

    file.open(path);
    file.close();

    return aidToString(currAid);
}

Auction getAuction(string aid) {
    string path = "AUCTIONS/" + aid + "/START_" + aid + ".txt";
    vector<string> input;
    string uid, aucName, Fname, stValue, duration, startSec, startDate1, startDate2;
    int state = 1;
    ifstream fin;
    ofstream fout;
    tm* endDate;
    int currSec, timeActive;
    time_t endSec;

    fin.open(path);
    if (!fin.good()) return Auction("!");       //aid nao existe

    fin >> uid >> aucName >> Fname >> stValue >> duration >> startDate1 >> startDate2 >> startSec;
    fin.close();

    // check if there is END file
    fin.open("AUCTIONS/" + aid + "/END_" + aid + ".txt");
    if (fin.good()) state = 0;       //ja existe END_(aid).txt
    fin.close();

    currSec = time(NULL);
    timeActive = getAuctionTimeActive(stoi(startSec), stoi(duration), currSec);
    if (timeActive >= stoi(duration) && state != 0) {       // auction (aid) has expired, add END_(aid).txt
        state = 0;
        endSec = stol(startSec) + stol(duration);
        endDate = gmtime(&endSec);
        fout.open("AUCTIONS/" + aid + "/END_" + aid + ".txt");
        fout << timeToString(endDate) << " " << duration;
        fout.close();
    }

    Auction a = Auction(stoi(uid), aucName, Fname, getHighestBid(aid), stoi(stValue), startDate1+" "+startDate2 , timeActive, aid, state);
    
    return a;
}

int endAuction(string aid) {
    ofstream fout;
    string endDate, endSec;    
    time_t fullTime;
    //int startSec, duration, currSec;
    
    Auction a = getAuction(aid);

    if (a._state == 0) return 0;       // aid already ended

    // end auction manually
    time(&fullTime);
    
    endSec = to_string(a._duration);
    endDate = timeToString(gmtime(&fullTime));

    // create END_(aid).txt
    fout.open("AUCTIONS/" + aid + "/END_" + aid + ".txt");
    fout << endDate << " " << endSec << '\n'; 
    fout.close();

    return 1;
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
    int l = id.length();

    if (l == 3) return id;
    else if (l == 2) return "0" + id;
    else if (l == 1) return "00" + id;
    else return "!"; 
}

string showAuctionRecord(string aid) {
    struct dirent **filelist;
    string name, path, bidDir = "AUCTIONS/"+aid+"/BIDS/";
    int numEntries;
    string uid, value, date, time, duration, res;
    ifstream fin;
    Auction a = getAuction(aid);

    res = a.toString();

    numEntries = scandir(bidDir.c_str(), &filelist, 0, alphasort);
    while (numEntries--) {
        name = filelist[numEntries]->d_name;
        if (name.length() > 4) {
            path = bidDir + name;
            res += "\n B ";
            fin.open(path, ios_base::app);
            fin >> uid >> value >> date >> time >> duration;
            fin.close();
            res += uid+" "+value+" "+date+" "+time+" "+duration+" ";
        }
        free(filelist[numEntries]);
    }
    free(filelist);

    if (a._state == 0) {
        path = "AUCTIONS/"+aid+"/END_"+aid+".txt";
        res += "\n E ";

        fin.open(path);
        fin >> date >> time >> duration;
        fin.close();

        res += date+" "+time+" "+duration+" ";
    }

    return res;
}   

int getAuctionStartValue(string aid) {
    ifstream fin;
    string stValue, irr, path = "AUCTIONS/" + aid + "/START_" + aid + ".txt";

    fin.open(path);
    if (!fin.good()) return -1;       //aid nao existe

    fin >> irr >> irr >> irr >> stValue >> irr >> irr >> irr >> irr;
    fin.close();
    return stoi(stValue);
}
// Bid Functions ----------------------------------------------------------------------

int getHighestBid(string aid) {
    struct dirent **filelist;
    string name, path, bidDir = "AUCTIONS/"+aid+"/BIDS/";
    int value, numEntries, highValue;
    highValue = getAuctionStartValue(aid);
    if (highValue == -1) return -1;         // aid não existe

    numEntries = scandir(bidDir.c_str(), &filelist, 0, alphasort);
    if (numEntries <= 2) return highValue;     //aid has no bids
    while (numEntries--) {
        name = filelist[numEntries]->d_name;
        if (name.length() > 4) {
            path = bidDir + name;

            value = stoi(name.substr(0, name.find_last_of(".")));
            if (value > highValue) highValue = value;
        }
        free(filelist[numEntries]);
    }
    free(filelist);
    return highValue;
}

int addBid(string uid, string aid, int value) {
    ofstream fout;
    time_t fullTime;
    tm *date;
    time(&fullTime);
    date = gmtime(&fullTime);
    
    // add auction to USERS/(uid)/BIDDED/
    fout.open("USERS/" + uid + "/BIDDED/" + aid + ".txt");
    fout.close();

    // add bid to AUCTIONS/BIDS/
    Auction a = getAuction(aid);
    fout.open("AUCTIONS/" + aid + "/BIDS/" + to_string(value) + ".txt");
    fout << uid << " " << to_string(value) << " " << timeToString(date) << " " << a._duration;
    fout.close();
    
    return 0;
}

