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
#include "pa2345.h"

#include "structs.h"

timestamp_t max_timestamp(timestamp_t a, timestamp_t b)
{
    return a > b ? a : b;
}

timestamp_t lamp_local_time = 0;
timestamp_t get_lamport_time()
{
    return lamp_local_time;
}

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
            lamp_local_time = max_timestamp(lamp_local_time, msg->s_header.s_local_time);
            ++lamp_local_time;
            
            return from;
        }
        else if(errno == EAGAIN)
        {
            for(int i = 0; i < 1000000; ++i) {} // handmade usleep analog!
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
        if((bytes_read = read(app->ppChannels[i][app->nProcessID].rf,
            &msg->s_header,
            sizeof(MessageHeader))) > 0)
        {
            bytes_read += read(app->ppChannels[i][app->nProcessID].rf,
                               msg->s_payload,
                               msg->s_header.s_payload_len);
            msg->s_payload[msg->s_header.s_payload_len] = 0;
            
            if(bytes_read == (sizeof(MessageHeader) + msg->s_header.s_payload_len))
            {
                lamp_local_time = max_timestamp(lamp_local_time, msg->s_header.s_local_time);
                ++lamp_local_time;
                
                return i;
            }
            else
            {
                printf("ERROR IN RECEIVE_ANY (NUMBER OF BYTES INEQUALITY)\n");
                return -1;
            }
        }
    }
    
    return -1;
}

int send(void * self, local_id dst, const Message * msg)
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
                     (msg->s_header.s_payload_len + sizeof(MessageHeader)))
            {
                printf("Error in sendmulticast\n");
            }
        }
    }
    
    return 0;
}

int request_cs(const void * self)
{
    DApp * app = (DApp*)self;
    
    // step 1: send request
    Message req_msg;
    req_msg.s_header.s_type = CS_REQUEST;
    ++lamp_local_time;
    req_msg.s_header.s_local_time = get_lamport_time();
    req_msg.s_header.s_payload_len = 0;
    req_msg.s_header.s_magic = MESSAGE_MAGIC;
    
    send_multicast(app, &req_msg);
    
    timestamp_t request_time = get_lamport_time();
    
    //printf("process %d sended request (priority: %d)\n", app->nProcessID, app->nProcessID + get_lamport_time());
    
    // step 2: add itself request to local queue
    pq_push(app->pQueue, request_time, app->nProcessID);

    // step 3: start receiving replies and requests
    char replies_received[app->nProcessesCount];
    memset(replies_received, 0, app->nProcessesCount);
    replies_received[app->nProcessID] = 1;
    
    for(int i = 1; i < app->nProcessesCount; ++i)
    {
        if(app->paProcessesStatuses[i] == DONE)
            replies_received[i] = 1;
    }
    // check that replies receiving is needed
    char replies_receiving = 0;
    for(int i = 1; i < app->nProcessesCount; ++i)
    {
        if(replies_received[i] == 0)
            replies_receiving = 1;
    }
    
    Message rec_msg;
    while(replies_receiving || (pq_peek(app->pQueue) != app->nProcessID))
    {
        int sender_id = receive_any(app, &rec_msg);
        if(sender_id == -1)
        {
            for(int i = 0; i < 1000000; ++i){} // homemade usleep analog
            continue;
        }
        
        if(rec_msg.s_header.s_type == CS_REQUEST)
        {
            pq_push(app->pQueue, rec_msg.s_header.s_local_time, sender_id);
            
            /*printf("process %d received request from %d with priority %d. queue size: %d top: %d\n", 
                   app->nProcessID,
                   sender_id, 
                   rec_msg.s_header.s_local_time,
                   app->pQueue->len,
                   pq_peek(app->pQueue)
                  );*/
            
            Message reply_msg;
            reply_msg.s_header.s_type = CS_REPLY;
            ++lamp_local_time;
            reply_msg.s_header.s_local_time = get_lamport_time();
            reply_msg.s_header.s_magic = MESSAGE_MAGIC;
            reply_msg.s_header.s_payload_len = 0;
            
            send(app, sender_id, &reply_msg);
        }
        else if(rec_msg.s_header.s_type == CS_REPLY)
        {
            //printf("process %d received reply from %d with time %d\n", app->nProcessID, sender_id, rec_msg.s_header.s_local_time);
            
            if(request_time < rec_msg.s_header.s_local_time)
            {
                replies_received[sender_id] = 1;
            }
            else
            {
                printf("ERROR: received YOUNGER MESSAGE\n");
            }
        }
        else if(rec_msg.s_header.s_type == CS_RELEASE)
        {
            if(!(pq_pop(app->pQueue) == sender_id))
            {
                printf("ERROR: popped not the sender_id request!\n");
            }
            
            /*printf("process %d received release from %d with time %d. queue size: %d top: %d\n", 
                   app->nProcessID, 
                   sender_id, 
                   rec_msg.s_header.s_local_time,
                   app->pQueue->len,
                   pq_peek(app->pQueue)
                  );*/
        }
        else if(rec_msg.s_header.s_type == DONE)
        {
            app->paProcessesStatuses[sender_id] = DONE;
            replies_received[sender_id] = 1;
        }
        
        // check that replies receiving is done
        replies_receiving = 0;
        for(int i = 1; i < app->nProcessesCount; ++i)
        {
            if(replies_received[i] == 0)
                replies_receiving = 1;
        }
        if(pq_peek(app->pQueue) == -1)
        {
            printf("ERROR: QUEUE IS NULL\n");
        }
    }
    
    return 0;
}

int release_cs(const void * self)
{
    DApp * app = (DApp*)self;
    
    //printf("process %d RELEASE cs\n", app->nProcessID);
    
    if(pq_pop(app->pQueue) == -1)
    {
        printf("ERROR: popping NULL queue\n");
    }
    
    Message rel_msg;
    rel_msg.s_header.s_type = CS_RELEASE;
    ++lamp_local_time;
    rel_msg.s_header.s_local_time = get_lamport_time();
    rel_msg.s_header.s_magic = MESSAGE_MAGIC;
    rel_msg.s_header.s_payload_len = 0;
    
    send_multicast(app, &rel_msg);
    
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
    for(int i = 1; i < app->nProcessesCount; ++i)
    {
        if(fork() == 0) // CHILD code execution starts here
        {
            app->nProcessID = i;
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
    app->paProcessesStatuses = (char*)malloc(app->nProcessesCount);
    memset(app->paProcessesStatuses, -1, app->nProcessesCount * sizeof(char));
    
    int  msg_len = 0;
    char msg_buf[4096];
    if(app->nProcessID == 0) // parent code
    {
            // log 'started'
        msg_len = sprintf(msg_buf,
                          log_started_fmt,
                          get_lamport_time(),
                          app->nProcessID,
                          app->nProcessPID,
                          app->nParentPID,
                          0
                         );
        fwrite(msg_buf, 1, msg_len, app->fLog);
        printf("%s", msg_buf);
        
        int starts_receiveing = 1;
        while(starts_receiveing)
        {
            int sender_id = 0;
            Message rmsg;
            
            if((sender_id = receive_any(app, &rmsg)) >= 0)
            {
                if(rmsg.s_header.s_type == STARTED)
                {
                    app->paProcessesStatuses[sender_id] = STARTED;
                    printf("%s", rmsg.s_payload);
                    fwrite(rmsg.s_payload, 1, rmsg.s_header.s_payload_len, app->fLog);
                }
            }
            else
            {
                for(int i = 0; i < 1000000; ++i){} // homemade usleep analog
            }
            
            starts_receiveing = 0;
            for(int i = 1; i < app->nProcessesCount; ++i)
            {
                if(app->paProcessesStatuses[i] != STARTED)
                    starts_receiveing = 1;
            }
        }
            // log 'received all started'
        msg_len = sprintf(msg_buf,
                          log_received_all_started_fmt,
                          get_lamport_time(),
                          app->nProcessID);
        fwrite(msg_buf, 1, msg_len, app->fLog);
        printf("%s", msg_buf);
        
            // work goes here
            // ...
            // work ends here
        
            // work done msg
        
        msg_len = sprintf(msg_buf,
                          log_done_fmt,
                          get_lamport_time(),
                          app->nProcessID,
                          0
                         );
        fwrite(msg_buf, 1, msg_len, app->fLog);
        printf("%s", msg_buf);
        
        int ends_receiveing = 1;
        while(ends_receiveing)
        {
            int sender_id = 0;
            Message rmsg;
            
            if((sender_id = receive_any(app, &rmsg)) >= 0)
            {
                if(rmsg.s_header.s_type == DONE)
                {
                    app->paProcessesStatuses[sender_id] = DONE;
                    //printf("%s", rmsg.s_payload);
                    fwrite(rmsg.s_payload, 1, rmsg.s_header.s_payload_len, app->fLog);
                }
            }
            else
            {
                for(int i = 0; i < 1000000; ++i){} // homemade usleep analog
            }
            
            ends_receiveing = 0;
            for(int i = 1; i < app->nProcessesCount; ++i)
            {
                if(app->paProcessesStatuses[i] != DONE)
                    ends_receiveing = 1;
            }
        }
        
            // all done
        msg_len = sprintf(msg_buf,
                          log_received_all_done_fmt,
                          get_lamport_time(),
                          app->nProcessID);
        fwrite(msg_buf, 1, msg_len, app->fLog);
        //printf("%s", msg_buf);
        
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
        // allocate priority queue
        app->pQueue = create_pqueue(50);
        
            // firstly - send STARTED to everyone
        Message msg;
        msg.s_header.s_type = STARTED;
        msg.s_header.s_magic = MESSAGE_MAGIC;
        ++lamp_local_time;
        msg.s_header.s_local_time = get_lamport_time();
        msg_len = sprintf(msg_buf,
                          log_started_fmt,
                          get_lamport_time(),
                          app->nProcessID,
                          app->nProcessPID,
                          app->nParentPID,
                          0
                         );
        
            // Mark process itself as started
        app->paProcessesStatuses[app->nProcessID] = STARTED;
        
            // send to other processes
        msg.s_header.s_payload_len = msg_len;
        memcpy(msg.s_payload, msg_buf, msg_len);
        send_multicast(app, &msg);
        
        int starts_receiveing = 1;
        while(starts_receiveing)
        {
            int sender_id = 0;
            Message rmsg;
            
            if((sender_id = receive_any(app, &rmsg)) >= 0)
            {
                if(rmsg.s_header.s_type == STARTED)
                {
                    app->paProcessesStatuses[sender_id] = STARTED;
                }
            } 
            else
            {
                for(int i = 0; i < 1000000; ++i){} // homemade usleep analog
            }
            
            starts_receiveing = 0;
            for(int i = 1; i < app->nProcessesCount; ++i)
            {
                if(app->paProcessesStatuses[i] != STARTED)
                    starts_receiveing = 1;
            }
        }
        
            // all started
        msg_len = sprintf(msg_buf,
                          log_received_all_started_fmt,
                          get_lamport_time(),
                          app->nProcessID);
        fwrite(msg_buf, 1, msg_len, app->fLog);
        //printf("%s", msg_buf);
        
            // work goes
        int iter_num = 1;
        int iter_count = app->nProcessID * 5;
        while(iter_num <= iter_count)
        {
            if(app->nMutexEnabled)
                request_cs(app);
            
            msg_len = sprintf(msg_buf,
                log_loop_operation_fmt,
                app->nProcessID,
                iter_num,
                iter_count
            );
            
            print(msg_buf);
            
            if(app->nMutexEnabled)
                release_cs(app);
            
            ++iter_num;
        }
        
            // work ends
        
            // work done msg
        msg_len = sprintf(msg_buf,
                          log_done_fmt,
                          get_lamport_time(),
                          app->nProcessID,
                          0
                         );
        
            // Mark process itself as done
        app->paProcessesStatuses[app->nProcessID] = DONE;
        
            // send to other processes
        msg.s_header.s_type = DONE;
        msg.s_header.s_payload_len = msg_len;
        ++lamp_local_time;
        msg.s_header.s_local_time = get_lamport_time();
        memcpy(msg.s_payload, msg_buf, msg_len);
        send_multicast(app, &msg);
        
        int ends_receiveing = 1;
        while(ends_receiveing)
        {
            int sender_id = 0;
            Message rmsg;
            
            if((sender_id = receive_any(app, &rmsg)) >= 0)
            {
                if(rmsg.s_header.s_type == DONE)
                {
                    app->paProcessesStatuses[sender_id] = DONE;
                }
            }
            else
            {
                for(int i = 0; i < 1000000; ++i){} // homemade usleep analog
            }
            
            ends_receiveing = 0;
            for(int i = 1; i < app->nProcessesCount; ++i)
            {
                if(app->paProcessesStatuses[i] != DONE)
                    ends_receiveing = 1;
            }
        }
        
            // all done
        msg_len = sprintf(msg_buf,
                          log_received_all_done_fmt,
                          get_lamport_time(),
                          app->nProcessID);
        //printf("%s", msg_buf);
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
    DApp app;
    app.nProcessID = 0; // everything starts from parent
    app.nMutexEnabled = 0;
    
    // check if mutexl is enabled
    for(int i = 1; i < argc; ++i)
    {
        if(!strcmp("--mutexl", argv[i]))
        {
            app.nMutexEnabled = 1;
        }
        else if(!strcmp("-p", argv[i]))
        {
            app.nProcessesCount = atoi(argv[i+1]) + 1;
        }
    }
    
    init_distr_app(&app);
    run_distr_app(&app);
    return 0;
}
