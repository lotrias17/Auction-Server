#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <sys/stat.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <dirent.h>
#include <vector>

#include "../userInc/Client.hpp"
#include "../asInc/Auction.hpp"

using namespace std;

int aid = 0;

int addUser(string uid, string password);
int unregisterUser(string uid);
int loginUser(string uid, string password);
int logoutUser(string uid);
int checkUserRegistry(string uid);
Client getUser(string uid);
int setUser(Client c);

vector<Auction> getUserAuctions(string uid, string dir);
string listAuctions(vector<Auction> list);
vector<Auction> getAllAuctions();
//int addAuction();
//int deleteAuction();
//int getAuction();

//int addBid();
//int getBid();


#endif