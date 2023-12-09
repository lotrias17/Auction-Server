#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <sys/stat.h>
#include <string>

using namespace std;

int emptyDir(string path);

int addUser(string uid, string password);
int deleteUser(string uid);
//int getUser();
//int loginUser(string uid, string password)
//int logoutUser(string uid)

//int addAuction();
//int deleteAuction();
//int getAuction();

//int addBid();
//int getBid();


#endif