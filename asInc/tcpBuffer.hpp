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
    tcpBuffer(int fd, int bufPos);
    void update(char* buffer);
    int _fd;
    int _bufPos = 0;
    string _buffer;
};

tcpBuffer *getTcpBuffer(int fd);
void removeTcp(int fd);

#endif