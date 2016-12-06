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
    sem_trywait(boss_sem);
    int err = errno;
    std::cout << "Scout received a right to write messages\n";
    
    while(dUserInput != 0)
    {
        std::cin >> dUserInput;
        switch (dUserInput)
        {
            case 1:
            case 2:
            {
//                send_integer(boss_port, dUserInput);
                break;
            }
            case 0:
            {
                sem_post(boss_sem);
                std::cout << "Scout process suicides\n";
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
