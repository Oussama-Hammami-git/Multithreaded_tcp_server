#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>


#define PATH_MAX    1000
void * Handle_Connections(void* pclientfd);


int main()
{
    /*
    NAME
        socket - create an endpoint for communication

    LIBRARY
        Standard C library (libc, -lc)

    SYNOPSIS
        #include <sys/socket.h>

        int socket(int domain, int type, int protocol);
    */
    int sockfd, status;

    sockfd = socket(AF_INET, SOCK_STREAM,0);
    if (sockfd <0)
    {
        perror("socket");
        return sockfd;
    }

    /*
    NAME
     connect - initiate a connection on a socket

    LIBRARY
     Standard C library (libc, -lc)

    SYNOPSIS
     #include <sys/socket.h>

     int connect(int sockfd, const struct sockaddr *addr,
                 socklen_t addrlen);
    */

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    // For server i will allow connection from any ip address
    addr.sin_addr.s_addr = INADDR_ANY;

    /*
    NAME
     bind - bind a name to a socket

    LIBRARY
     Standard C library (libc, -lc)

    SYNOPSIS
     #include <sys/socket.h>

     int bind(int sockfd, const struct sockaddr *addr,
              socklen_t addrlen);
    */

    status = bind(sockfd, (struct sockaddr*)& addr, sizeof(addr));
    if(status)
    {
        perror("bind");
        close(sockfd);
        return status;
    }

    /*
    NAME
     listen - listen for connections on a socket

    LIBRARY
     Standard C library (libc, -lc)

    SYNOPSIS
     #include <sys/socket.h>

     int listen(int sockfd, int backlog);
    */

    status = listen(sockfd, 1);
    if(status)
    {
        perror("listen");
        close(sockfd);
        return status;
    }

    /*
    NAME
     accept, accept4 - accept a connection on a socket

    LIBRARY
     Standard C library (libc, -lc)

    SYNOPSIS
     #include <sys/socket.h>

     int accept(int sockfd, struct sockaddr *_Nullable restrict addr,
                socklen_t *_Nullable restrict addrlen);

     #define _GNU_SOURCE             See feature_test_macros(7)
     #include <sys/socket.h>

     int accept4(int sockfd, struct sockaddr *_Nullable restrict addr,
                socklen_t *_Nullable restrict addrlen, int flags);

    */
    while(1)
    {
        int clientfd;
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len);

        if(clientfd < 0)
        {
            perror("accept");
            close(sockfd);
            return clientfd;
        }

        printf("client port : %d , client ip address %s\n", client_addr.sin_port, inet_ntoa(client_addr.sin_addr));

        pthread_t t;
        void* pclient = malloc(sizeof(int));
        *(int*)pclient = clientfd;
        pthread_create(&t, NULL, Handle_Connections, pclient);
    }
    close(sockfd);
    return 0;

}

    void * Handle_Connections(void* pclientfd)
    {
        int client_socket = *(int*)pclientfd;
        free(pclientfd);
        char buff[255];
        ssize_t bytes_read;
        int msgSize = 0;
        char actualPath[PATH_MAX];
    
        /* Read client's message until newline or buffer is full */
        while (msgSize < (int)sizeof(buff) - 1)
        {
            bytes_read = read(client_socket, buff + msgSize, sizeof(buff) - 1 - msgSize);
            if (bytes_read <= 0) break;
            msgSize += bytes_read;
            if (buff[msgSize - 1] == '\n') break;
        }
    
        if (msgSize <= 0) {
            close(client_socket);
            return NULL;
        }
    
        /* Strip newline and null-terminate */
        if (buff[msgSize - 1] == '\n') msgSize--;
        if (msgSize > 0 && buff[msgSize - 1] == '\r') msgSize--;
        buff[msgSize] = '\0';
    
        printf("request : %s\n", buff);
        fflush(stdout);
    
        if (realpath(buff, actualPath) == NULL)
        {
            perror("path error");
            close(client_socket);
            return NULL;
        }

        FILE *fp = fopen(actualPath, "r");
        if (fp == NULL)
        {
            perror("failed to open file");
            close(client_socket);
            return NULL;
        }

        size_t n;
        while ((n = fread(buff, 1, sizeof(buff), fp)) > 0)
        {
            //printf("sending %zu bytes\n", n);
            write(client_socket, buff, n);   
        }
        close(client_socket);
        fclose(fp);
        return NULL;
    }

// nc -l -p 12345