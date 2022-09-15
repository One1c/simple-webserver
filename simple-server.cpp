#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <errno.h>
#include <malloc.h>
#include <signal.h>
#include <iostream>
#include <sys/wait.h>
#include <sys/types.h>

#define BUFF_SIZE 30000
#define PORT 80

char* parse_method(char* line, const char* symbol);
char* parse(char* line, const char* symbol);
void send_message(int fd, char image_path[], char head[]);
void endchild(int);
int main(int argc, char* argv[])
{
    int servfd, clientfd;
    struct sockaddr_in servaddr, clientaddr;
    char BUFF[BUFF_SIZE];
    char* MESSAGE = nullptr;
    socklen_t addrlen = sizeof(clientaddr);
    char buff[BUFF_SIZE];

    servfd = socket(AF_INET, SOCK_STREAM, 0);
    int reuse = 1;
    setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    signal(SIGCHLD, endchild);

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    bind(servfd, (struct sockaddr*)&servaddr, addrlen);

    listen(servfd, 5);

    char* http_head = "HTTP/1.1 200 Ok\r\n";

    while(true)
    {
        clientfd = accept(servfd, (struct sockaddr *)&clientaddr, &addrlen);
		printf("+++++++++++++++++++++ Waiting for a new onnection ++++++++++++++++++\n");
        int pid = fork();

        if(pid == 0)
        {
			printf("Child Process started\n");
            read(clientfd, BUFF, BUFF_SIZE);
            printf("------- HTTP request header -------\n%s--------------------------------------\n",BUFF);

            //++++++++++++++ 获取request的方式 parse_sring_method +++++++++++++
            char* parse_string_method = parse_method(BUFF, " ");
            printf("Requested method: %s\n", parse_string_method);

            //++++++++++++++ 获取request的目录 parse_string  +++++++++++++
            char* parse_string = parse(BUFF, " ");
            printf("Requested path: %s\n", parse_string);

            //++++++++++++++ 获取文件后缀名 parse_ext +++++++++++++
            char* copy = (char* )malloc(strlen(parse_string) + 1);
            strcpy(copy, parse_string);
            char* parse_ext = parse(copy, ".");
            
			//+++++++++++++ 构建response头 copy_head +++++++++++++++++
            char* copy_head = (char*)malloc(strlen(http_head)+200);
            strcpy(copy_head, http_head);;
     
				
					
			if(strncmp(parse_string_method, (char*)"GET", 3) == 0)
            {
                char path_head[50] = ".";				
                if(strlen(parse_string) <= 1)
                {
                    strcat(path_head, "/index.html");
				   	strcat(copy_head, "Content-type: text/html\r\n\r\n");
                    send_message(clientfd, path_head, copy_head);
                }
				else if(strcasecmp(parse_ext, "html") == 0)
				{
                    strcat(path_head, parse_string);
                    strcat(copy_head, "Content-type: text/html\r\n\r\n");
                    send_message(clientfd, path_head, copy_head);
				}	
				else if(strcasecmp(parse_ext, "jpeg") == 0 || strcasecmp(parse_ext, "jpg") == 0)
				{
					strcat(path_head, parse_string);
					strcat(copy_head, "Content-type: image/jpeg\r\n\r\n");
					send_message(clientfd, path_head, copy_head);
				}
				else if(strcasecmp(parse_ext, "ico") == 0)
				{
					strcat(path_head, "/favicon.png");
					strcat(copy_head, "Content-type: image/vnd.microsoft.icon\r\n\r\n");
					send_message(clientfd, path_head, copy_head);
				}
				else if(strcasecmp(parse_ext, "css") == 0)
				{
					strcat(path_head, parse_string);
					strcat(copy_head, "Content-type: text/css\r\n\r\n");
					send_message(clientfd, path_head, copy_head);
				}				
				else if(strcasecmp(parse_ext, "js") == 0)
				{
					strcat(path_head, parse_string);
					strcat(copy_head, "Content-type: text/javascript\r\n\r\n");
					send_message(clientfd, path_head, copy_head);
				}
				else
				{
                    strcat(path_head, "/404.html");
                    strcat(copy_head, "Content-type: text/html\r\n\r\n");
                    send_message(clientfd, path_head, copy_head);
				}
						
        
            }
            
            close(clientfd);
			printf("CLIENT DISCONNECTED\n");
			return 0;
			
        }
        else
        {
			
            printf("Parent process create child %d\n", pid);
            close(clientfd);
        }
    }
    return 0;
}

char* parse_method(char* line, const char* symbol)
{
    char *copy = (char *)malloc(strlen(line) + 1);
    strcpy(copy, line);
    char* str = strtok(copy, symbol);

    char* message;
    int count = 0;
    while(str != NULL)
    {
        if(count == 0)
        {
            message = str;
            if(message == NULL)
            {
                message = "";
            }
            break;
        }
    }
    return message;
}

char* parse(char* line, const char* symbol)
{
    char* str = strtok(line, symbol);
    char* message;
    int count = 0;
    while(str != NULL)
    {
        str = strtok(NULL, symbol);
        if(count == 0)
        {
            message = str;
            if(message == NULL)
            {
                message = "";
            }
            break;
        }
    }
    return message;
}

void send_message(int fd, char image_path[], char head[])
{

    struct stat stat_buf;
	bool status = true;
	if(stat(image_path, &stat_buf) != 0){
		strcpy(image_path, "./404.html");
		strcpy(head,"HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n");
		status = false;	

	}
	
    write(fd, head, strlen(head));
    int fdimg = open(image_path, O_RDONLY);

    fstat(fdimg, &stat_buf);

    int img_total_size = stat_buf.st_size;
    int block_size = stat_buf.st_blksize;

    int sent_size;

    while(img_total_size > 0){
        if(img_total_size < block_size){
            sent_size = sendfile(fd, fdimg, NULL, img_total_size);            
        }
        else{
            sent_size = sendfile(fd, fdimg, NULL, block_size);
        }       
        img_total_size = img_total_size - sent_size;
    }
    close(fdimg);


}

void endchild(int sig)
{
    pid_t pid;
    int status;
    pid = waitpid(-1, &status, WNOHANG);
    printf("Child id killed: %d \n\n\n\n", pid); 	    
	  
}

