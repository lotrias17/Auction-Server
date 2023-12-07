#ifndef AS_HPP
#define AS_HPP

#include "../userSrc/Client.cpp"

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
#define PORT "58011"    //should be 58000 + Group Number

int fd,errcode;
ssize_t n;
socklen_t addrlen;
struct addrinfo hints,*res;
struct sockaddr_in addr;
char buffer[128];
unordered_map<string, Client*> userList;

void receiveRequest();
int serverResponse(char* buf);
bool isAlphaNumeric(string str);
bool isNumeric(string str);
bool checkFormat(string format, string str);
int processLogin(string uid, string password);
int processLogout(string uid);
int processUnregister(string uid);



#endif