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

#include "database.hpp"
#include "tcpBuffer.hpp"

using namespace std;

int port = 58014;
int newfd, ufd, tfd, udpErrcode, tcpErrcode, outFds;
ssize_t n;
socklen_t addrlen;
struct addrinfo uhints,*ures, thints, *tres;
struct sockaddr_in udpAddr, tcpAddr;
bool verbose = false;
fd_set inputs, testFds;

void verboseOut(vector<string> input, string protocol);
void setUdpSocket(char* p);
void setTcpSocket(char* p);
void receiveRequest();
bool checkPORTFormat(char* str);
int serverResponse(string buffer, string protocol);
bool isAlphaNumeric(string str);
bool isNumeric(string str);
bool checkFormat(string format, string str);

int processLogin(string uid, string password);
int processLogout(string uid);
int processUnregister(string uid);
int processListMyAuctions(string uid);
int processListMyBids(string uid);
int processShowRecord(string aid);
int processList();

int processOpen(vector<string> input);
int processClose(string uid, string aid);
int processBid(string uid, string aid, string bid);
int processShowAsset();



#endif