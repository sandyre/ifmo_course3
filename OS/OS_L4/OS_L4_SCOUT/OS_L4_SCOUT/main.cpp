//
//  main.cpp
//  OS_L4_SCOUT
//
//  Created by Aleksandr Borzikh on 04.12.16.
//  Copyright © 2016 sandyre. All rights reserved.
//

#include <iostream>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define BOSS_SEMAPHORE_NAME "/boss_semaphore"

int main(int argc, char * argv[])
{
    if(argc != 2)
    {
        std::cout << "Wrong number of agruments passed: " << argc
                  << "Abort.\n";
        return 1;
    }
    
    std::cout << "Scout process started!\n";
    std::cout << "PID: " << getpid() << "\n\n";
    std::cout << "What do you want to send to Boss?\n"
              << "Available input: [1,2,0 = exit]\n";
    std::cout << "Boss IPC port: " << argv[1] << "\n";
    int dUserInput = -1;
    mach_port_t boss_port = atoi(argv[1]);
    
    sem_t * boss_sem = sem_open(BOSS_SEMAPHORE_NAME, 0);
    sem_wait(boss_sem);
    int err = errno;
    std::cout << "Scout received a right to write messages\n";
    
    char buffer[256] = { 0 };
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr;
    struct hostent * server;
    
    server = gethostbyname("localhost");
    if(server == nullptr)
    {
        std::cout << "Error dereferencing localhost";
        sem_post(boss_sem);
        return 1;
    }
    
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = 1337;
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        std::cout << "Error connecting via tcp socket";
        sem_post(boss_sem);
        return 2;
    }
    
    while(dUserInput != 0)
    {
        std::cin >> dUserInput;
        switch (dUserInput)
        {
            case 1:
            {
                write(sockfd, "1", strlen("1"));
                break;
            }
            case 2:
            {
                write(sockfd, "2", strlen("2"));
                break;
            }
            case 0:
            {
                sem_post(boss_sem);
                std::cout << "Scout process suicides\n";
                close(sockfd);
                return 0;
                break;
            }
            default:
            {
                std::cout << "Wrong argument passed. Available input: [1,2,0 = exit]\n";
                break;
            }
        }
    }
}
