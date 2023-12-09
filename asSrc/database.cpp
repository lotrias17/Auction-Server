#include "../asInc/database.hpp"

int emptyDir(string path) {
    (void) path;
    return 0;
}

int addUser(string id, string password) {
    //create txt files for user data
    //check for auctions with same uid, add those to HOSTED
    //check for bids with same uid, add those to BIDDED

    (void) password;

    int uid = stoi(id);
    char userDir[15];
    char auxDir[25];

    //if user doesnt exist already


    if (uid < 1 || uid >= 1000000) {    //invalid uid
        return 0;
    }

    sprintf(userDir, "USERS/%06d", uid);
    if ((mkdir(userDir, 0700)) == -1) return -1;

    sprintf(auxDir, "USERS/%06d/HOSTED", uid);
    if ((mkdir(auxDir, 0700)) == -1) {
        // empty userDir and then
        rmdir(userDir);
        return -1;
    }

    sprintf(auxDir, "USERS/%06d/BIDDED", uid);
    if ((mkdir(auxDir, 0700)) == -1) {
        // empty userDir and then
        rmdir(userDir);
        return -1;
    }
    
    return 1;
}

int deleteUser(string id) {
    int uid = stoi(id);
    char userDir[15];

    if (uid < 1 || uid >= 1000000) {    //invalid uid
        return 0;
    }

    sprintf(userDir, "USERS/%06d", uid);

    // empty userDir and then
    rmdir(userDir);
    return 1;
}
