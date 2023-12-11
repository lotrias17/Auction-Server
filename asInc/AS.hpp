#ifndef AS_HPP
#define AS_HPP

#include <unordered_map>
#include <vector>
#include <cstring>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>  

#include "../userSrc/Client.cpp"

#include "Auction.hpp"
#include "database.hpp"

using namespace std;

int port = 58011;   //should be 58000 + Group Number
int ufd, tfd, udpErrcode, tcpErrcode, outFds;
int aid = 0;
ssize_t n;
socklen_t addrlen;
struct addrinfo uhints,*ures, thints, *tres;
struct sockaddr_in udpAddr, tcpAddr;
bool verbose = false;
unordered_map<string, Client*> userList;
fd_set inputs, testFds;

void verboseOut(vector<string> input, string protocol);
void setUdpSocket(char* p);
void setTcpSocket(char* p);
void receiveRequest();
bool checkPORTFormat(char* str);
int serverResponse(char* buffer, string protocol);
bool isAlphaNumeric(string str);
bool isNumeric(string str);
bool checkFormat(string format, string str);

int processLogin(string uid, string password);
int processLogout(string uid);
int processUnregister(string uid);
int processListMyAuctions(string uid);
int processListMyBids(string uid);
int processList();
int show_record();

int processOpen();
int processClose();
int processBid();
int processShowAsset();



#endif