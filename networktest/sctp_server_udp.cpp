#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/sctp.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <cstring>
#include <stdio.h>
#include <iostream>
using namespace std;

#define MAXLINE 100

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

int main()
{
    // udp style
    int sockfd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
    if (sockfd < 0)
    {
        perror("socket");
        return 0;
    }

    /* use all the available addrs
    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(7777);

    if (bind(sockfd, (sockaddr*)&servaddr, sizeof(servaddr))< 0)
    {
        perror("bind");
        return 0;
    }
    //*/

    vector<sockaddr_in> addrs(10);
    sockaddr_in* pAddr = &addrs[0];
    //* specify the addr set
    addrs[0] = buildAddr("192.168.1.11");
    addrs[1] = buildAddr("192.168.1.12");
    addrs[2] = buildAddr("192.168.1.13");
    if (sctp_bindx(sockfd, (sockaddr*)pAddr, 3, SCTP_BINDX_ADD_ADDR) == -1)
    {
        perror("sctp_bindx");
        return 0;
    }
    //*/

    int addr_count = sctp_getladdrs(sockfd, 0, (sockaddr**)&pAddr);
    for (int n = 0; n < addr_count; n++) 
    {
        cout<<"local addr: "<<inet_ntoa(addrs[n].sin_addr)<<", port: "<<ntohs(addrs[n].sin_port)<<endl;
    }

    if (listen(sockfd, 1) < 0)
    {
        perror("listen");
        return 0;
    }

    sctp_sndrcvinfo sinfo;
    bzero(&sinfo, sizeof(sinfo));
    int flags;

    // set events
    sctp_event_subscribe events; 
    bzero(&events, sizeof(events)); 
    events.sctp_data_io_event = 1; // enable sinfo to be filled
    events.sctp_association_event = 1; // if a new association has started or if an old one has terminated
    events.sctp_shutdown_event = 1; // remote peer has shut down
    if (setsockopt(sockfd, IPPROTO_SCTP, SCTP_EVENTS, &events, sizeof(events)) < 0)
    {
        perror("setsockopt");
        return 0;
    }


    char recvline[MAXLINE + 1];
    string msg;
    int sno = 0;
    while (1)
    {
        // make a small buffer to test recieving a complete msg for multiple times 
        // the MSG_EOR indicates a complete sctp_send()
        int n = sctp_recvmsg(sockfd, recvline, MAXLINE, 0, 0, &sinfo, &flags);
        if (flags & MSG_NOTIFICATION) // handle event
        {
            sctp_notification* notification = (sctp_notification*)recvline; 
            switch (notification->sn_header.sn_type) 
            { 
            case SCTP_SHUTDOWN_EVENT:
                { 
                    sctp_shutdown_event* shut = (sctp_shutdown_event*)recvline; 
                    cout<<"Shutdown on assoc id: "<<shut->sse_assoc_id<<endl;
                    break; 
                }
            case SCTP_ASSOC_CHANGE:
                {
                    sctp_assoc_change* change = (sctp_assoc_change*)recvline;
                    cout<<"Begin/End on assoc id: "<<change->sac_assoc_id<<endl;
                    break;
                }
            default: 
                cout<<"Unhandled event type: "<<notification->sn_header.sn_type<<endl;
            }
        }
        else // handle data
        {
            if (n > 0)
            {
                recvline[n] = 0;
                msg += recvline;
                cout<<"recvd: "<<recvline<<", from stream: "<<sinfo.sinfo_stream<<", assoc id: "<<sinfo.sinfo_assoc_id<<", flags: "<<flags<<endl;

                if (flags & MSG_EOR) 
                {
                    cout<<"reached End Of Record"<<endl;
                    // send in the specific assoc/stream, while write() does not care. 
                    // "sinfo.sinfo_flags |= SCTP_SENDALL" can be used to send on all assoc.
                    // "sinfo.sinfo_flags |= SCTP_UNORDERED" can be used to send unordered in this stream
                    sinfo.sinfo_stream = sno++ % 5; // send to different streams
                    if (sctp_send(sockfd, msg.c_str(), msg.size(), &sinfo, 0) < 0)
                    {
                        perror("sctp_send");
                        break;
                    }
                    msg.clear();
                }
            }
            else if (n < 0)   
            {
                perror("read");
                break;
            }
            else // n == 0
            {
                cout << "read EOF" << endl;
                break;
            }
        }
    }

    close(sockfd);  
    return 0;
}
