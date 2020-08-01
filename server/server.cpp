#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <errno.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <iostream>
#include "threadpool.cpp"
#include "../common/constants.cpp"
using namespace std;

int main(int argc, char **argv) {
    int main_socket, new_socket, addrlen, activity, getval, sd, max_sd, i = 0, options, count = 0, threads = 20, numSetsInCache = 2, sizeOfSet = 4;
    int client_list[50];
    struct sockaddr_in address;
    fd_set incomingfds;

    if(argc > 1) {
        for(int i=1; i<argc; i++) {
            if(strncmp(argv[i], "-threadPoolSize=", strlen("-threadPoolSize=")) == 0) {
                threads = atoi(argv[i] + strlen("-threadPoolSize="));
            } else if(strncmp(argv[i], "-numSetsInCache=", strlen("-numSetsInCache=")) == 0) {
                numSetsInCache = atoi(argv[i] + strlen("-numSetsInCache="));
            } else if(strncmp(argv[i], "-sizeOfSet=", strlen("-sizeOfSet=")) == 0) {
                sizeOfSet = atoi(argv[i] + strlen("-sizeOfSet="));
            }
        }
    }

    while (i < MAX_CLIENT) {
        client_list[i] = 0;
        i++;
    }
    main_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (main_socket == -1) {
        perror("Network Error:Could not create socket\n");
        exit(-1);
    }
    if (setsockopt(main_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &options, sizeof(options)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_port = htons(HTONS_PORT);
    address.sin_addr.s_addr = 0;

    if (bind(main_socket, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind()");
        exit(1);
    }
    //intializing threadpool===================
    struct pool *p = create_pool(threads);
    initialize_cache(sizeOfSet, numSetsInCache);
    initiliase_store();

    if (listen(main_socket, 50) != 0) {
        perror("listen()");
        exit(1);
    }
    addrlen = sizeof(address);
    printf("Waiting for connection...\n");
    char *Buffer = (char *) malloc((MAX_BUFFER_SIZE+ GEN_BUFFER_SIZE));
    while (1) {
        //printf("1\n");
        FD_ZERO(&incomingfds);
        FD_SET(main_socket, &incomingfds);
        max_sd = main_socket;
        for (i = 0; i < MAX_CLIENT; i++) {
            if (client_list[i] > 0) {
                FD_SET(client_list[i], &incomingfds);
            }
            if (client_list[i] > max_sd) {
                max_sd = client_list[i];
            }
        }
        //printf("1.5\n");
        activity = select(max_sd + 1, &incomingfds, NULL, NULL, NULL);
        //printf("2\n");
        if (activity < 0 && (errno != EINTR)) {
            printf("select error");
        }
        if (FD_ISSET(main_socket, &incomingfds)) {
            if ((new_socket = accept(main_socket, (struct sockaddr *) &address, (socklen_t * ) & addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            for (i = 0; i < MAX_CLIENT; i++) {
                //if position is empty  
                if (client_list[i] == 0) {
                    client_list[i] = new_socket;
                    //printf("Adding to list of sockets as %d\n", i);
                    count++;
                    break;
                }
            }
        }
        //printf("3\n");
        //else its some IO operation on some other socket 
        for (i = 0; i < MAX_CLIENT; i++) {
            sd = client_list[i];

            if (FD_ISSET(sd, &incomingfds)) {
                //Check if it was for closing, and also read the incoming msg
                memset(Buffer, 0, sizeof(Buffer));
                getval = read(sd, Buffer, (MAX_BUFFER_SIZE+GEN_BUFFER_SIZE));
                if (getval == 0) {
                    //Somebody disconnected , get his details and print  
                    getpeername(sd, (struct sockaddr *) &address, (socklen_t * ) & addrlen);
                    // printf("Host disconnected , ip %s , port %d \n" ,inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                    count--;
                    //Close the socket and mark as 0 in list for reuse  
                    close(sd);
                    client_list[i] = 0;
                } else {
                    //printf("server resv==%s\n",Buffer);
                    place_request(p, Buffer, sd);
                }
            }
        }
    }
    return 0;
}