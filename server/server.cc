#include "protocol.hpp"

int main()
{
    int sockfd = UDP::Create();
    UDP::Bind(sockfd, 8777);
    struct sockaddr_in peer_addr;

    UDP::RecvVideo(sockfd, "video.mp4", peer_addr);
    return 0;
}

