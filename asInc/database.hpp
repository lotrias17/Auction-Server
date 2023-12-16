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
#include <time.h>
#include <fstream>

#include "../userInc/Client.hpp"
#include "../asInc/Auction.hpp"
#include "../asInc/Bid.hpp"

using namespace std;

string aidToString(int aid);
string timeToString(tm* time);
string bidToString(int value);
int checkUserRegistry(string uid);
string showAuctionRecord(string aid);
int getAuctionStartValue(string aid);

int addUser(string uid, string password);
int unregisterUser(string uid);
int loginUser(string uid, string password);
int logoutUser(string uid);
Client getUser(string uid);
int setUser(Client c);

vector<Auction> getUserAuctions(string uid, string dir);
string listAuctions(vector<Auction> list);
vector<Auction> getAllAuctions();
string addAuction(vector<string> input);
Auction getAuction(string aid);
int endAuction(string aid);

int addBid(string uid, string aid, int value);
int getHighestBid(string aid);
//int addBid();
//int getBid();


#endif