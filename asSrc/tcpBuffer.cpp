#include "../asInc/tcpBuffer.hpp"

vector<tcpBuffer> tcpList;

tcpBuffer::tcpBuffer() {
    _fd = -1;
    _bufPos = 0;
}

tcpBuffer::tcpBuffer(int fd, int bufPos) {
    _fd = fd;
    _bufPos = bufPos;
}

void tcpBuffer::update(char* buf) {
    _buffer += buf;
    _bufPos += 3;
}

void removeTcp(int fd) {
    for (int i = 0; i < (int) tcpList.size(); i++) {
        if (tcpList[i]._fd == fd) tcpList.erase(tcpList.begin()+i);
    }
    return;
}

tcpBuffer *getTcpBuffer(int fd) {
    for (int i = 0; i < (int) tcpList.size(); i++) {
        if (tcpList[i]._fd == fd) return &tcpList[i];
    }
    tcpBuffer t = tcpBuffer(fd, 0);
    tcpList.push_back(t);
    return &tcpList[tcpList.size()-1];
}