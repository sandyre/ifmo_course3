//
//  main.c
//  pa1
//
//  Created by Aleksandr Borzikh on 20.03.17.
//  Copyright © 2017 hate-red. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <time.h>
#include "ipc.h"
#include "common.h"
#include "pa1.h"

typedef struct
{
    int rf;
    int wf;
} Channel;

typedef struct
{
    pid_t      nProcessPID;
    pid_t      nParentPID;
    int        nProcessID; // current Node ID, 0 for parent, > 0 for child
    int        nParentID;
    int        nProcessesCount;
    FILE *     fLog; // file descriptor for logging
    FILE *     fPipesLog;
    Channel ** ppChannels; // N x N
} DApp;

int receive(void * self, local_id from, Message * msg)
{
    DApp * app = (DApp*)self;
    
    while(1)
    {
        ssize_t bytes_read = 0;
        if((bytes_read = read(app->ppChannels[from][app->nProcessID].rf,
                              &msg->s_header,
                              sizeof(MessageHeader))) > 0)
        {
            bytes_read += read(app->ppChannels[from][app->nProcessID].rf,
                               msg->s_payload,
                               msg->s_header.s_payload_len);
            msg->s_payload[msg->s_header.s_payload_len] = 0;
        }
        
        if(bytes_read == (sizeof(MessageHeader) + msg->s_header.s_payload_len))
        {
            return 0;
        }
        else if(errno == EAGAIN)
        {
            for(int i = 0; i < 100000; ++i) {} // handmade usleep analog!
        }
        else
        {
            printf("Error occured in receive!\n");
            return -1;
        }
    }
}

int receive_any(void * self, Message * msg)
{
    DApp * app = (DApp*)self;
    ssize_t bytes_read = 0;
    
    for(int i = 0; i < app->nProcessesCount; ++i)
    {
        if(i == app->nProcessID) continue; // skip current node
        
        bytes_read = read(app->ppChannels[i][app->nProcessID].rf,
                          msg,
                          MAX_MESSAGE_LEN);
        
        if(bytes_read > 0)
        {
            return 0;
        }
        else if(bytes_read == -1 &&
                errno == EAGAIN)
        {
            if(i == app->nProcessesCount-1)
            {
                i = 0;
            }
            else
            {
                continue;
            }
        }
        else
        {
            return -1;
        }
    }
    
    return -1;
}

int send_one(void * self, local_id dst, const Message * msg)
{
    DApp * app = (DApp*)self;
    
    write(app->ppChannels[app->nProcessID][dst].wf,
          msg,
          msg->s_header.s_payload_len + sizeof(MessageHeader));
    
    return 0;
}

int send_multicast(void * self, const Message * msg)
{
    DApp * app = (DApp*)self;
    
    for(int i = 0; i < app->nProcessesCount; ++i)
    {
        if(i != app->nProcessID)
        {
            if(write(app->ppChannels[app->nProcessID][i].wf,
                     msg,
                     msg->s_header.s_payload_len + sizeof(MessageHeader)) !=
                     msg->s_header.s_payload_len + sizeof(MessageHeader))
            {
                printf("Error in sendmulticast\n");
            }
        }
    }
    
    return 0;
}

void init_distr_app(DApp * app)
{
        // open
    app->nProcessPID = getpid();
    app->nParentPID = 0; // parent has no parent
    
    app->fLog = fopen(events_log, "a");
    app->fPipesLog = fopen(pipes_log, "w");
    
        // allocate pipes array
    app->ppChannels = calloc(app->nProcessesCount, sizeof(Channel*));
    for(int i = 0; i < app->nProcessesCount; ++i)
    {
        app->ppChannels[i] = calloc(app->nProcessesCount, sizeof(Channel));
    }
    
        // init pipes
    for(int i = 0; i < app->nProcessesCount; ++i)
    {
        for(int j = 0; j < app->nProcessesCount; ++j)
        {
            if(i == j) continue; // I -> I communication refused
            
            if(pipe((int*)&app->ppChannels[i][j]) != -1)
            {
                fcntl(app->ppChannels[i][j].rf, F_SETFL, O_NONBLOCK);
                fcntl(app->ppChannels[i][j].wf, F_SETFL, O_NONBLOCK);
                printf("Pipe set %d -> %d\n", i, j);
                fprintf(app->fPipesLog, "Pipe set %d -> %d\n", i, j);
            }
            else
            {
                printf("Error creating pipes\n");
                exit(-1);
            }
        }
    }
    
    fclose(app->fPipesLog);
    
        // create child processes
    pid_t ParentPID = getpid();
    for(int i = 0; i < app->nProcessesCount - 1; ++i)
    {
        if(fork() == 0) // CHILD code execution starts here
        {
            app->nProcessID = i + 1;
            app->nProcessPID = getpid();
            app->nParentPID = ParentPID;
            break;
        }
    }
    
        // configure pipes (personally)
    close(app->ppChannels[app->nProcessID][app->nProcessID].rf);
    close(app->ppChannels[app->nProcessID][app->nProcessID].wf);
    
    for(int i = 0; i < app->nProcessesCount; ++i)
    {
        for(int j = 0; j < app->nProcessesCount; ++j)
        {
            if(i != app->nProcessID &&
               j != app->nProcessID)
            {
                close(app->ppChannels[i][j].rf);
                close(app->ppChannels[i][j].wf);
            }
            if(i == app->nProcessID &&
               j != app->nProcessID)
            {
                close(app->ppChannels[i][j].rf);
            }
            if(j == app->nProcessID &&
               i != app->nProcessID)
            {
                close(app->ppChannels[i][j].wf);
            }
        }
    }
}

void run_distr_app(DApp * app)
{
    int  processes_statuses[app->nProcessesCount];
    memset(processes_statuses, -1, (app->nProcessesCount) * sizeof(int));
    int  msg_len = 0;
    char msg_buf[4096];
    if(app->nProcessID == 0) // parent code
    {
            // log 'started'
        msg_len = sprintf(msg_buf,
                          log_started_fmt,
                          app->nProcessID,
                          app->nProcessPID,
                          app->nParentPID);
        fwrite(msg_buf, 1, msg_len, app->fLog);
        printf("%s", msg_buf);
        
        int starts_receiveing = 1;
        while(starts_receiveing)
        {
            for(int i = 1; i < app->nProcessesCount; ++i)
            {
                if(processes_statuses[i] == STARTED)
                    continue;
                
                Message msg;
                if(receive(app, i, &msg) == 0)
                {
                    if(msg.s_header.s_type == STARTED)
                    {
                        processes_statuses[i] = STARTED;
                        printf("%s", msg.s_payload);
                        fwrite(msg.s_payload, 1, msg.s_header.s_payload_len, app->fLog);
                    }
                    else
                    {
                        printf("Error occured in parent\n");
                    }
                }
            }
            
            starts_receiveing = 0;
            for(int i = 1; i < app->nProcessesCount; ++i)
            {
                if(processes_statuses[i] != STARTED)
                    starts_receiveing = 1;
            }
        }
            // log 'received all started'
        msg_len = sprintf(msg_buf,
                          log_received_all_started_fmt,
                          app->nProcessID);
        fwrite(msg_buf, 1, msg_len, app->fLog);
        printf("%s", msg_buf);
        
            // work goes here
            // ...
            // work ends here
        
            // work done msg
        
        msg_len = sprintf(msg_buf,
                          log_done_fmt,
                          app->nProcessID);
        fwrite(msg_buf, 1, msg_len, app->fLog);
        printf("%s", msg_buf);
        
        int ends_receiveing = 1;
        while(ends_receiveing)
        {
            for(int i = 1; i < app->nProcessesCount; ++i)
            {
                if(processes_statuses[i] == DONE)
                    continue;
                
                Message msg;
                if(receive(app, i, &msg) == 0)
                {
                    if(msg.s_header.s_type == DONE)
                    {
                        processes_statuses[i] = DONE;
                        printf("%s", msg.s_payload);
                        fwrite(msg.s_payload, 1, msg.s_header.s_payload_len,app->fLog);
                    }
                    else
                    {
                        printf("Error occured in parent\n");
                    }
                }
            }
            
            ends_receiveing = 0;
            for(int i = 1; i < app->nProcessesCount; ++i)
            {
                if(processes_statuses[i] != DONE)
                    ends_receiveing = 1;
            }
        }
        
            // all done
        msg_len = sprintf(msg_buf,
                          log_received_all_done_fmt,
                          app->nProcessID);
        fwrite(msg_buf, 1, msg_len, app->fLog);
        printf("%s", msg_buf);
        
        int waiting_procs = app->nProcessesCount-1;
        while(waiting_procs)
        {
            wait(NULL);
            --waiting_procs;
        }
        
            // close everything
        fclose(app->fLog);
        for(int i = 0; i < app->nProcessesCount; ++i)
        {
            for(int j = 0; j < app->nProcessesCount; ++j)
            {
                close(app->ppChannels[i][j].rf);
                close(app->ppChannels[i][j].wf);
            }
            free(app->ppChannels[i]);
        }
        free(app->ppChannels);
        
        exit(0); // everything worked fine!
    }
    else // child code
    {
            // firstly - send STARTED to everyone
        Message msg;
        msg.s_header.s_type = STARTED;
        msg.s_header.s_magic = MESSAGE_MAGIC;
        msg.s_header.s_local_time = time(NULL);
        msg_len = sprintf(msg_buf,
                          log_started_fmt,
                          app->nProcessID,
                          app->nProcessPID,
                          app->nParentPID);
        
            // Mark process itself as started
        processes_statuses[app->nProcessID] = STARTED;
        
            // send to other processes
        msg.s_header.s_payload_len = msg_len;
        memcpy(msg.s_payload, msg_buf, msg_len);
        send_multicast(app, &msg);
        
        int starts_receiveing = 1;
        while(starts_receiveing)
        {
            for(int i = 1; i < app->nProcessesCount; ++i)
            {
                if(processes_statuses[i] == STARTED)
                    continue;
                
                Message rmsg;
                if(receive(app, i, &rmsg) == 0)
                {
                    if(rmsg.s_header.s_type == STARTED)
                    {
                        processes_statuses[i] = STARTED;
                    }
                    else
                    {
                        printf("1: Error occured in child %d, received %d\n", app->nProcessID, rmsg.s_header.s_type);
                    }
                }
            }
            
            starts_receiveing = 0;
            for(int i = 1; i < app->nProcessesCount; ++i)
            {
                if(processes_statuses[i] != STARTED)
                    starts_receiveing = 1;
            }
        }
        
            // all started
        msg_len = sprintf(msg_buf,
                          log_received_all_started_fmt,
                          app->nProcessID);
        fwrite(msg_buf, 1, msg_len, app->fLog);
        printf("%s", msg_buf);
        
            // work goes here
            // ...
            // work ends here
        
            // work done msg
        msg_len = sprintf(msg_buf,
                          log_done_fmt,
                          app->nProcessID);
        
            // Mark process itself as done
        processes_statuses[app->nProcessID] = DONE;
        
            // send to other processes
        msg.s_header.s_type = DONE;
        msg.s_header.s_payload_len = msg_len;
        msg.s_header.s_local_time = time(NULL);
        memcpy(msg.s_payload, msg_buf, msg_len);
        send_multicast(app, &msg);
        
        int ends_receiveing = 1;
        while(ends_receiveing)
        {
            for(int i = 1; i < app->nProcessesCount; ++i)
            {
                if(processes_statuses[i] == DONE)
                    continue;
                
                Message rmsg;
                if(receive(app, i, &rmsg) == 0)
                {
                    if(rmsg.s_header.s_type == DONE)
                    {
                        processes_statuses[i] = DONE;
                    }
                    else
                    {
                        printf("2: Error occured in child %d, received %d\n", app->nProcessID, rmsg.s_header.s_type);
                    }
                }
            }
            
            ends_receiveing = 0;
            for(int i = 1; i < app->nProcessesCount; ++i)
            {
                if(processes_statuses[i] != DONE)
                    ends_receiveing = 1;
            }
        }
        
            // all done
        msg_len = sprintf(msg_buf,
                          log_received_all_done_fmt,
                          app->nProcessID);
        printf("%s", msg_buf);
        fwrite(msg_buf, 1, msg_len, app->fLog);
        
            // close everything
        fclose(app->fLog);
        for(int i = 0; i < app->nProcessesCount; ++i)
        {
            for(int j = 0; j < app->nProcessesCount; ++j)
            {
                close(app->ppChannels[i][j].rf);
                close(app->ppChannels[i][j].wf);
            }
            free(app->ppChannels[i]);
        }
        free(app->ppChannels);
        exit(0);
    }
}

int main(int argc, const char * argv[])
{
    if(argc < 3 || strcmp(argv[1], "-p") !=0)
        return 1;
    
    DApp app;
    app.nProcessesCount = atoi(argv[2]) + 1;
    app.nProcessID = 0; // everything starts from parent
    
    init_distr_app(&app);
    run_distr_app(&app);
    return 0;
}
