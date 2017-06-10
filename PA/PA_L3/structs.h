#ifndef __STRUCTS_H__
#define __STRUCTS_H__

#include "banking.h"

#include <stdio.h>
#include <fcntl.h>

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
    
    balance_t  nBalance;
} DApp;

#endif
