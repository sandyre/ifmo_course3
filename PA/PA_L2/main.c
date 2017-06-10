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

void transfer(void * parent_data, local_id src, local_id dst, balance_t amount)
{
    DApp * app = (DApp*)parent_data;
    
    // create message and send it to src
    Message msg;
    msg.s_header.s_local_time = get_physical_time();
    msg.s_header.s_magic = MESSAGE_MAGIC;
    msg.s_header.s_type = TRANSFER;
    msg.s_header.s_payload_len = sizeof(TransferOrder);
    
    TransferOrder transf;
    transf.s_dst = dst;
    transf.s_src = src;
    transf.s_amount = amount;
    memcpy(msg.s_payload, &transf, sizeof(transf));
    
    send(app, src, &msg);
    if(receive(app, dst, &msg) == 0 &&
        msg.s_header.s_type != ACK)
    {
        printf("%d: ERROR parent received garbage from %d\n", get_physical_time(),
            dst
        );
    }
}

void init_distr_app(DApp * app, int * initial_balances)
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
    app->nBalance = initial_balances[0];
    for(int i = 0; i < app->nProcessesCount - 1; ++i)
    {
        if(fork() == 0) // CHILD code execution starts here
        {
            app->nProcessID = i + 1;
            app->nProcessPID = getpid();
            app->nParentPID = ParentPID;
            app->nBalance = initial_balances[app->nProcessID];
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
        AllHistory history;
        history.s_history_len = app->nProcessesCount-1;
        
            // log 'started'
        msg_len = sprintf(msg_buf,
                          log_started_fmt,
                          get_physical_time(),
                          app->nProcessID,
                          app->nProcessPID,
                          app->nParentPID,
                          app->nBalance
                         );
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
                          get_physical_time(),
                          app->nProcessID);
        fwrite(msg_buf, 1, msg_len, app->fLog);
        printf("%s", msg_buf);
        
            // work goes here
        bank_robbery(app, app->nProcessesCount-1);
        Message stop_msg;
        stop_msg.s_header.s_type = STOP;
        stop_msg.s_header.s_magic = MESSAGE_MAGIC;
        stop_msg.s_header.s_payload_len = 0;
        stop_msg.s_header.s_local_time = get_physical_time();
        
        send_multicast(app, &stop_msg);
            // work ends here
        
            // work done msg
        
        msg_len = sprintf(msg_buf,
                          log_done_fmt,
                          get_physical_time(),
                          app->nProcessID,
                          app->nBalance
                         );
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
                          get_physical_time(),
                          app->nProcessID);
        fwrite(msg_buf, 1, msg_len, app->fLog);
        printf("%s", msg_buf);
        
        int histories_remaining = app->nProcessesCount-1;
        printf("Parent starts to receive histories\n");
        while(histories_remaining)
        {
            Message history_msg;
            if(receive_any(app, &history_msg) == 0 &&
                history_msg.s_header.s_type == BALANCE_HISTORY)
            {
                BalanceHistory * pBH = (BalanceHistory*)history_msg.s_payload;
                memcpy(&history.s_history[pBH->s_id-1],
                    history_msg.s_payload,
                    history_msg.s_header.s_payload_len
                );
                histories_remaining--;
            }
            else
            {
                print("parent received garbage in historyloop\n");
            }
        }
        
        printf("Parent received all histories\n");
        print_history(&history);
        
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
            // initialize balance history
        BalanceHistory history;
        history.s_id = app->nProcessID;
        
        timestamp_t last_history_row = get_physical_time();
        history.s_history[0].s_balance_pending_in = 0;
        history.s_history[0].s_balance = app->nBalance;
        history.s_history[0].s_time = get_physical_time();
        
            // firstly - send STARTED to everyone
        Message msg;
        msg.s_header.s_type = STARTED;
        msg.s_header.s_magic = MESSAGE_MAGIC;
        msg.s_header.s_local_time = get_physical_time();
        msg_len = sprintf(msg_buf,
                          log_started_fmt,
                          get_physical_time(),
                          app->nProcessID,
                          app->nProcessPID,
                          app->nParentPID,
                          app->nBalance
                         );
        
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
                          get_physical_time(),
                          app->nProcessID);
        fwrite(msg_buf, 1, msg_len, app->fLog);
        printf("%s", msg_buf);
        
            // initialize balance history
        
            // work goes here
        Message rmsg;
        while(1)
        {
            if(receive_any(app, &rmsg) == 0)
            {
                if(rmsg.s_header.s_type == TRANSFER)
                {
                    TransferOrder * order = (TransferOrder*)rmsg.s_payload;
                    
                    if(order->s_src == app->nProcessID)
                    {
                        app->nBalance -= order->s_amount;
                        
                        timestamp_t cur_time = get_physical_time();
                        last_history_row = cur_time;
                        history.s_history[cur_time].s_time = cur_time;
                        history.s_history[cur_time].s_balance = app->nBalance;
                        history.s_history[cur_time].s_balance_pending_in = 0;
                        
                        msg_len = sprintf(msg_buf,
                            log_transfer_out_fmt,
                            cur_time,
                            app->nProcessID,
                            order->s_amount,
                            order->s_dst
                        );
                        fwrite(msg_buf, 1, msg_len, app->fLog);
                        printf("%s", msg_buf);
                        
                        send(app, order->s_dst, &rmsg);
                    }
                    else if(order->s_dst == app->nProcessID)
                    {
                        app->nBalance += order->s_amount;
                        
                        timestamp_t cur_time = get_physical_time();
                        last_history_row = cur_time;
                        history.s_history[cur_time].s_time = cur_time;
                        history.s_history[cur_time].s_balance = app->nBalance;
                        history.s_history[cur_time].s_balance_pending_in = 0;
                        
                        msg_len = sprintf(msg_buf,
                            log_transfer_in_fmt,
                            cur_time,
                            app->nProcessID,
                            order->s_amount,
                            order->s_src
                        );
                        fwrite(msg_buf, 1, msg_len, app->fLog);
                        printf("%s", msg_buf);
                        
                        Message conf_msg;
                        conf_msg.s_header.s_type = ACK;
                        conf_msg.s_header.s_local_time = cur_time;
                        conf_msg.s_header.s_magic = MESSAGE_MAGIC;
                        conf_msg.s_header.s_payload_len = 0;
                        
                        send(app, 0, &conf_msg);
                    }
                    else
                    {
                        printf("VERY BAD ERROR\n");   
                    }
                }
                else if(rmsg.s_header.s_type == STOP)
                {
                    break;
                }
                else
                {
                    printf("VERY BAD ERROR!\n");
                }
            }
        }
            // work ends here
        
            // work done msg
        msg_len = sprintf(msg_buf,
                          log_done_fmt,
                          get_physical_time(),
                          app->nProcessID,
                          app->nBalance
                         );
        
            // Mark process itself as done
        processes_statuses[app->nProcessID] = DONE;
        
            // send to other processes
        msg.s_header.s_type = DONE;
        msg.s_header.s_payload_len = msg_len;
        msg.s_header.s_local_time = get_physical_time();
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
                          get_physical_time(),
                          app->nProcessID);
        printf("%s", msg_buf);
        fwrite(msg_buf, 1, msg_len, app->fLog);
        
            // prepare history & send it
        timestamp_t cur_time = get_physical_time();
        last_history_row = cur_time;
        history.s_history[cur_time].s_time = cur_time;
        history.s_history[cur_time].s_balance = app->nBalance;
        history.s_history[cur_time].s_balance_pending_in = 0;
        
        history.s_history_len = last_history_row + 1;
        balance_t cur_bal = history.s_history[0].s_balance;
        cur_time = history.s_history[0].s_time;
        for(int i = 1; i < history.s_history_len; ++i)
        {
            ++cur_time;
            if(history.s_history[i].s_balance == 0 &&
                history.s_history[i].s_time == 0)
            {
                history.s_history[i].s_balance = cur_bal;
                history.s_history[i].s_time = cur_time;
            }
            else
            {
                cur_bal = history.s_history[i].s_balance;
                cur_time = history.s_history[i].s_time;
            }
        }
        
            // send it
        Message history_msg;
        history_msg.s_header.s_type = BALANCE_HISTORY;
        history_msg.s_header.s_magic = MESSAGE_MAGIC;
        history_msg.s_header.s_local_time = get_physical_time();
        history_msg.s_header.s_payload_len = sizeof(history.s_id) + sizeof(history.s_history_len) + history.s_history_len * sizeof(BalanceState);
        
        memcpy(history_msg.s_payload, &history, history_msg.s_header.s_payload_len);
        
        send(app, 0, &history_msg);
        
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
    
    int initial_balances[app.nProcessesCount];
    initial_balances[0] = 0;
    for(int i = 1; i < app.nProcessesCount; ++i)
    {
        initial_balances[i] = atoi(argv[i+2]);
    }
    
    init_distr_app(&app, initial_balances);
    run_distr_app(&app);
    return 0;
}
