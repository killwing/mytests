#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <cstring>
using namespace std;
#define MAXLINE 1000

sockaddr_in buildAddr(const string& s)
{
    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(7777);
    if (inet_pton(AF_INET, s.c_str(), &servaddr.sin_addr) <= 0)
    {
        perror("inet_pton");
    }
    return servaddr;
}

sctp_assoc_t get_associd(int sockfd, sockaddr* sa, socklen_t salen) 
{ 
    sctp_paddrinfo sp; 
    int sz; 
    
    sz = sizeof(sctp_paddrinfo); 
    bzero(&sp, sz); 
    memcpy(&sp.spinfo_address, sa, salen); 
    if (sctp_opt_info(sockfd, 0, SCTP_GET_PEER_ADDR_INFO, &sp, (socklen_t*)&sz) == -1) 
    {
        perror("sctp_opt_info"); 
    }
    return (sp.spinfo_assoc_id); 
}

int main()
{ 
    int socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
    if (socketfd < 0)
    {
        perror("socket");
        return 0;
    }

    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(7777);
    if (inet_pton(AF_INET, "192.168.1.11", &servaddr.sin_addr) <= 0)
    {
        perror("inet_pton");
        return 0;
    }

    //* specify the addr set, default use all addrs available
    vector<sockaddr_in> addrs(10);
    sockaddr_in* pAddr = &addrs[0];
    addrs[0] = buildAddr("192.168.1.21");
    addrs[1] = buildAddr("192.168.1.22");
    addrs[2] = buildAddr("192.168.1.23");
    if (sctp_bindx(socketfd, (sockaddr*)pAddr, 3, SCTP_BINDX_ADD_ADDR) == -1)
    {
        perror("sctp_bindx");
        return 0;
    }

    int addr_count = sctp_getladdrs(socketfd, 0, (sockaddr**)&pAddr);
    for (int n = 0; n < addr_count; n++) 
    {
        cout<<"local addr: "<<inet_ntoa(addrs[n].sin_addr)<<", port: "<<ntohs(addrs[n].sin_port)<<endl;
    }
    //*/

    // set expect output/input streams
    sctp_initmsg initmsg;
    memset(&initmsg, 0, sizeof(initmsg));
    initmsg.sinit_num_ostreams = 8; // default 10
    initmsg.sinit_max_instreams = 50; // default 65535
    if (setsockopt(socketfd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(initmsg)) < 0) 
    {
        perror("setsockopt");
        return 0;
    }

    // auto bind local addresses available & ephemeral port (multihoming automatically)
    // use connectx() to connect specific server's ip set. 
    if (connect(socketfd, (sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("connect");
        return 0;
    }

    // get negotiated output/input streams
    sctp_status status;
    int len = sizeof(status);
    memset(&status, 0, len);
    if (getsockopt(socketfd, IPPROTO_SCTP, SCTP_STATUS, &status, (socklen_t*)&len) == -1) 
    {
        perror("getsockopt");
        return 0;
    }
    cout<<"Got assoc id: "<<status.sstat_assoc_id<<", in streams: "<<status.sstat_instrms<<", out streams: "<<status.sstat_outstrms<<endl;

    // get assoc id by dest ip
    //cout<<"assoc id: "<<get_associd(socketfd, (sockaddr*)&servaddr, sizeof(servaddr))<<endl;

    // which of the remote peer's interface it would prefer to use, 
    if (inet_pton(AF_INET, "192.168.1.12", &servaddr.sin_addr) <= 0)
    {
        perror("inet_pton");
        return 0;
    }
    sctp_setprim setprim;
    sockaddr_storage* ss = (sockaddr_storage*)&servaddr;
    setprim.ssp_assoc_id = status.sstat_assoc_id;
    setprim.ssp_addr = *ss;
    if (setsockopt(socketfd, IPPROTO_SCTP, SCTP_PRIMARY_ADDR, &setprim, sizeof(sctp_setprim)) < 0)
    {
        perror("setsockopt");
        return 0;
    }

    // on which interfaces it would prefer to receive 
    // need to enable /proc/sys/net/sctp/addip_enable or use sysctl()
    sctp_setpeerprim setpeerprim;
    sockaddr_storage* pss = (sockaddr_storage*)&addrs[1]; // 192.168.1.22
    setpeerprim.sspp_assoc_id = 0;//status.sstat_assoc_id;
    setpeerprim.sspp_addr = *pss;
    if (setsockopt(socketfd, IPPROTO_SCTP, SCTP_SET_PEER_PRIMARY_ADDR, &setpeerprim, sizeof(sctp_setpeerprim)) < 0)
    {
        perror("setsockopt");
        return 0;
    }

    // send in stream 5
    sctp_sndrcvinfo sinfo;
    bzero(&sinfo, sizeof(sinfo));
    sinfo.sinfo_stream = 5;

    char recvline[MAXLINE + 1];
    while (1)
    {
        string str;
        cin >> str;
        if (!cin.good())
        {
            cout << "I/O: input end or error" << endl;
            // Initiate graceful shutdown process, on the specific assoc
            sinfo.sinfo_flags = SCTP_EOF;
            sctp_send(socketfd, NULL, 0, &sinfo, 0);
            break;
        }

        // send in the specific stream, while write() does not care.
        if (sctp_send(socketfd, str.c_str(), str.size(), &sinfo, 0) < 0)
        {
            perror("sctp_send");
            break;
        }

        // read() can receive from any stream
        int n = read(socketfd, recvline, MAXLINE);
        if (n > 0)
        {
            recvline[n] = 0;
            cout << recvline << endl;
        }
        else if (n < 0)   
        {
            perror("read");
            break;
        }
        else
        {
            cout << "read EOF" << endl;
            break;
        }
    }

    // close all the assoc on this socket
    close(socketfd);

    return 0;
}

