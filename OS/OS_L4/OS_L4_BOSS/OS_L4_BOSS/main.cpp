//
//  main.cpp
//  OS_L4_BOSS
//
//  Created by Aleksandr Borzikh on 04.12.16.
//  Copyright © 2016 sandyre. All rights reserved.
//

#include <iostream>
#include <unistd.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <mach/mach.h>
#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/message.h>
#include <mach/mig_errors.h>
#include <mach/vm_statistics.h>

#define BOSS_SEMAPHORE_NAME "/boss_semaphore"

int main(int argc, const char * argv[])
{
    unsigned int nScoutsCount = 0;
    
    kern_return_t err;
    mach_port_t boss_rcv_port;
    err = mach_port_allocate(mach_task_self(),
                             MACH_PORT_RIGHT_RECEIVE,
                             &boss_rcv_port);
    err = mach_port_insert_right(mach_task_self(),
                                 boss_rcv_port,
                                 boss_rcv_port,
                                 MACH_MSG_TYPE_MAKE_SEND);
    sem_t * boss_sem = sem_open(BOSS_SEMAPHORE_NAME,
                                O_CREAT, 0644, 2);
    sem_close(boss_sem);
    std::cout << "Boss process started.\n"
              << "PID: " << getpid() << "\n";
    std::cout << "Enter number of scouts to launch: ";
    std::cin >> nScoutsCount;
    
    std::string scoutLaunchCommand =
    "/usr/bin/osascript -e 'tell app \"Terminal\" to do script \"/users/aleksandr/Desktop/OS_L4_SCOUT ";
    scoutLaunchCommand += std::to_string(boss_rcv_port);
    scoutLaunchCommand += "\"'";
    for(auto i = 0; i < nScoutsCount; ++i)
    {
        system(scoutLaunchCommand.c_str());
    }
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr, client_addr;
    socklen_t cli_len = sizeof(client_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = 1337;
    if(bind(sockfd, (struct sockaddr*)&serv_addr,
            sizeof(serv_addr)) < 0)
    {
        std::cout << "Error binding port\n";
    }
    
    while(true)
    {
        int msglen = 0;
        char buffer[256] = { 0 };
        listen(sockfd, 5);
        int nsockfd = accept(sockfd, (struct sockaddr*)&client_addr,
                             &cli_len);
        
        msglen = read(nsockfd, buffer, 255);
        
        if(!strcmp("1", buffer))
        {
            std::cout << "Scout sent you msg \"1\"\n";
        }
        else if(!strcmp("2", buffer))
        {
            std::cout << "Scout sent you msg \"2\"\n";
        }
    }
}
