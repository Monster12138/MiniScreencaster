#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <thread>
#include "TimeStr.h"
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

void threadMain(SocketPair sockPair)
{
    char timeStr[15];
    getNowTime(timeStr);
    char fileName[30];
    sprintf(fileName, "./log/%s.log", timeStr);
    FILE* fp = fopen(fileName, "w");
    int len;
    int recvBytes, sendBytes;
    char flag = 1;
    uchar recvbuf[1000000];
    sockPair.senderSocket.Send(&flag, sizeof(char));
    while(1)
    {
        int ret = sockPair.senderSocket.Recv(&len, sizeof(int));
        if(0 >= ret)
            break;
        ret = sockPair.recverSocket.Send(&len, sizeof(int));
        if(0 >= ret)
            break;
        fprintf(fp, "len: %d\n", len);
        //std::cout << "len :" << len << std::endl;

        recvBytes = 0;
        sendBytes = 0;

        ret = sockPair.senderSocket.Send(&flag, sizeof(char));
        if(0 >= ret)
            break;

        while(recvBytes < len)
        {
            int recverRet = sockPair.senderSocket.Recv(recvbuf + recvBytes, len - recvBytes);
            if(0 >= recverRet)
            {
                goto done;
            }
            recvBytes += recverRet;
            fprintf(fp, "Recv %d Bytes data\n", recvBytes);
            //std::cout << "Recv " <<recvBytes << " Bytes data\n";
        }

        
        while(sendBytes < len)
        {
            int senderRet = sockPair.recverSocket.Send(recvbuf + sendBytes, len - sendBytes);
            if(0 >= senderRet)
            {
                goto done;
            }
            sendBytes += senderRet;
            fprintf(fp, "Send %d Bytes data\n", sendBytes);
            //std::cout << "Send " << sendBytes << " Bytes data"<< std::endl;
        }

        fprintf(fp, "/=========== Forward %d Bytes data===========/\n", sendBytes);
        //std::cout << "==========================Forward " << sendBytes << " Bytes data\n";
    }
done:
    fclose(fp);
    sockPair.senderSocket.Close();
    sockPair.recverSocket.Close();
    cout << "/*************** a thread quit ****************/\n";
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
        cout << "Wait sender...\n";
        sockpair.senderSocket.setSocket(servSocket.Accept((sockaddr*)&senderAddr, len));

        memcpy(&sAddr.ip, &senderAddr.sin_addr.s_addr, sizeof(in_addr));
        sAddr.port = senderAddr.sin_port;
        cout << "Sender ip:" << inet_ntoa(sAddr.ip) << " port:" << sAddr.port << endl;
        sockpair.senderSocket.Send(&sAddr, sizeof(Addr));

        cout << "Wait receiver...\n";
        sockpair.recverSocket.setSocket(servSocket.Accept((sockaddr*)&recverAddr, len));
        memcpy(&rAddr.ip, &recverAddr.sin_addr.s_addr, sizeof(in_addr));
        rAddr.port = recverAddr.sin_port;
        cout << "Recver ip:" << inet_ntoa(rAddr.ip) << " port:" << rAddr.port << endl;
        sockpair.recverSocket.Send(&sAddr, sizeof(Addr));
        
        thread newThread{threadMain, sockpair};
        newThread.detach();
        cout << "/*************** new thread run ****************/\n";
    }
    servSocket.Close();
    return 0;
}

