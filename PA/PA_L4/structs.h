#ifndef __STRUCTS_H__
#define __STRUCTS_H__

#include <stdio.h>
#include <fcntl.h>

typedef struct
{
    int priority;
    int data;
} pqnode;

typedef struct
{
    pqnode * nodes;
    int len;
    int size;
} pqueue;

pqueue * create_pqueue(int size)
{
    pqueue * pq = (pqueue*)calloc(1, sizeof(pqueue));
    pq->size = size;
    pq->len = 0;
    pq->nodes = (pqnode*)calloc(size, sizeof(pqnode));
    
    return pq;
}

void pq_push(pqueue * pq, int priority, int data)
{
    if(!pq->len)
    {
        pq->nodes[0].priority = priority;
        pq->nodes[0].data = data;
        pq->len++;
    }
    else
    {
        int ins_idx = 0;
        while(1)
        {
                // its the end
            if(ins_idx == pq->len)
            {
                pq->nodes[ins_idx].priority = priority;
                pq->nodes[ins_idx].data = data;
                pq->len++;
                return;
            }
            else
            {
                if((pq->nodes[ins_idx].priority > priority) ||
                   (pq->nodes[ins_idx].priority == priority &&
                    pq->nodes[ins_idx].data > data))
                {
                        // shift to right
                    for(int i = pq->len; i > ins_idx; --i)
                    {
                        pq->nodes[i].priority = pq->nodes[i-1].priority;
                        pq->nodes[i].data = pq->nodes[i-1].data;
                    }
                        // insertion
                    pq->nodes[ins_idx].priority = priority;
                    pq->nodes[ins_idx].data = data;
                    pq->len++;
                    return;
                }
                ++ins_idx;
            }
        }
    }
}

int pq_pop(pqueue * pq)
{
    if(!pq->len)
    {
        return -1;
    }
    int data = pq->nodes[0].data;
        // shift left
    for(int i = 0; i < pq->len; ++i)
    {
        pq->nodes[i].priority = pq->nodes[i+1].priority;
        pq->nodes[i].data = pq->nodes[i+1].data;
    }
    pq->len--;
    
    return data;
}

int pq_peek(pqueue * pq)
{
    if(!pq->len)
    {
        return -1;
    }
    return pq->nodes[0].data;
}

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
    char       nMutexEnabled;
    pqueue *   pQueue;
    char *     paProcessesStatuses;
} DApp;

#endif
