#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <thread>
#include "Socket.hpp"
using namespace std;

struct Addr
{
    struct in_addr  ip;
    int port;
};

struct SocketPair
{
    Socket senderSocket;
    Socket recverSocket;
    SocketPair():senderSocket(0), recverSocket(0) {}
    SocketPair(Socket ssock, Socket rsock):senderSocket(ssock), recverSocket(rsock) {}
    SocketPair(int ssockfd, int rsockfd):senderSocket(ssockfd), recverSocket(rsockfd) {}
};

typedef unsigned char uchar;
uchar recvbuf[1000000];

void threadMain(SocketPair sockPair)
{
    int len;
    int recvBytes, sendBytes;
    char flag = 1;
    sockPair.senderSocket.Send(&flag, sizeof(char));
    while(1)
    {
        sockPair.senderSocket.Recv(&len, sizeof(int));
        sockPair.recverSocket.Send(&len, sizeof(int));
        recvBytes = 0;
        sendBytes = 0;
        std::cout << "len :" << len << std::endl;
        sockPair.senderSocket.Send(&flag, sizeof(char));
        while(recvBytes < len)
        {
            int recverRet = sockPair.senderSocket.Recv(recvbuf + recvBytes, len - recvBytes);
            if(0 >= recverRet)
                break;
            recvBytes += recverRet;
            std::cout << "Recv " <<recvBytes << " Bytes data\n";
        }

        
        while(sendBytes < len)
        {
            std::cout << "Sending...\n";
            int senderRet = sockPair.recverSocket.Send(recvbuf + sendBytes, len - sendBytes);
            if(0 >= senderRet)
                break;
            sendBytes += senderRet;
            std::cout << "Send " << sendBytes << " Bytes data"<< std::endl;
        }

        std::cout << "==========================Forward " << sendBytes << " Bytes data\n";
    }
    sockPair.senderSocket.Close();
    sockPair.recverSocket.Close();
}

int main()
{
    Socket servSocket;

    int contain;
    Addr sAddr, rAddr;

    servSocket.Create(AF_INET, SOCK_STREAM, 0);
    setsockopt(servSocket.getSocket(), SOL_SOCKET, SO_REUSEADDR, (char *)&contain, sizeof(int));
    servSocket.Bind(8888);
    servSocket.Listen(5);

    sockaddr_in senderAddr, recverAddr;

    bzero(&senderAddr, sizeof(sockaddr_in));
    socklen_t len = sizeof(senderAddr);

    SocketPair sockpair;
    while(1)
    {
        sockpair.senderSocket.setSocket(servSocket.Accept((sockaddr*)&senderAddr, len));

        memcpy(&sAddr.ip, &senderAddr.sin_addr.s_addr, sizeof(in_addr));
        sAddr.port = senderAddr.sin_port;
        cout << "Sender ip:" << inet_ntoa(sAddr.ip) << " port:" << sAddr.port << endl;
        sockpair.senderSocket.Send(&sAddr, sizeof(Addr));
        cout << "Send sAddr finish\n";

        sockpair.recverSocket.setSocket(servSocket.Accept((sockaddr*)&recverAddr, len));
        memcpy(&rAddr.ip, &recverAddr.sin_addr.s_addr, sizeof(in_addr));
        rAddr.port = recverAddr.sin_port;
        cout << "Recver ip:" << inet_ntoa(rAddr.ip) << " port:" << rAddr.port << endl;
        sockpair.recverSocket.Send(&sAddr, sizeof(Addr));
        cout << "Send sAddr finish\n";
        
        thread newThread{threadMain, sockpair};
        newThread.detach();
    }
    servSocket.Close();
    return 0;
}

