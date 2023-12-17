#ifndef TCPBUFFER_HPP
#define TCPBUFFER_HPP

#include <vector>
#include <stdlib.h>
#include <cstring>
#include <string>

using namespace std;

class tcpBuffer {
    public:
    tcpBuffer();
    tcpBuffer(int fd);
    void update(char* buffer);
    int _fd;
    bool isOpa = false;
    int _fsize;
    string _aid, _excess, _path, _necessary;
    string _buffer;
    bool canWrite, toCreate;
};

tcpBuffer *getTcpBuffer(int fd);
void removeTcp(int fd);

#endif