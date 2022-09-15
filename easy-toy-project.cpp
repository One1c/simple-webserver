#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#define BUFF_SIZE 30000
#define PORT 80

using std::cout;
using std::endl;



int main(int argc, char* argv[])
{
    int servfd, clientfd;
    struct sockaddr_in servaddr, clientaddr;
    char BUFF[BUFF_SIZE];
    char* MESSAGE = nullptr;
    socklen_t addrlen = sizeof(clientaddr);

    servfd = socket(AF_INET, SOCK_STREAM, 0);

    int reuse = 1;
    setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    bind(servfd, (struct sockaddr*)&servaddr, addrlen);

    listen(servfd, 5);

    while(true)
    {
        cout<<"\n+++++++++++ Waiting for an new connection ++++++++++\n"<<endl;
        clientfd = accept(servfd, (struct sockaddr*)&clientaddr, &addrlen);
        cout<<"------------------------------------------------------\nVistor IP: "<<inet_ntoa(clientaddr.sin_addr)<<endl;
        read(clientfd, BUFF, BUFF_SIZE);
        cout<<BUFF<<"------------------------------------------"<<endl;
        MESSAGE = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
        write(clientfd, (void* )MESSAGE, strlen(MESSAGE));
        close(clientfd);
    }
    return 0;
    


}