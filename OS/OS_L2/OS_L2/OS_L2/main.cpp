//
//  main.cpp
//  OS_L2
//
//  Created by Aleksandr Borzikh on 06.11.16.
//  Copyright © 2016 sandyre. All rights reserved.
//

#include <iostream>
#include <thread>
#include <chrono>
#include <queue>

#define PROCESS_TACT 1
float gCurrentTime = 0.0f;

class Process
{
public:
    enum ProcessState
    {
        RUNNING = 0x1,
        FINISHED = 0x2
    };
    
    Process() : m_fRemainingRunningTime(0.0f),
                m_eState(RUNNING)
    {
    }
    
    void  MakeStep()
    {
        if( m_fRemainingRunningTime - 1.0f == 0 )
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(PROCESS_TACT));
            m_fRemainingRunningTime -= 1.0f;
            m_eState = FINISHED;
        }
        else
        {
            m_fRemainingRunningTime -= 1.0f;
            std::this_thread::sleep_for(std::chrono::milliseconds(PROCESS_TACT));
        }
    }
    
    ProcessState GetProcessState() const
    {
        return m_eState;
    }
    
    float GetRemainingRunningTime() const
    {
        return m_fRemainingRunningTime;
    }
    void  SetRemainingRunningTime(float fValue)
    {
        m_fRemainingRunningTime = fValue;
    }
    
protected:
    enum ProcessState m_eState;
    float m_fRemainingRunningTime;
};

enum ProcessManagerState
{
    RUNNING = 0x1,
    FINISHED = 0x2
};

class RR_Manager
{
public:
    RR_Manager(float fQuant) : m_fQuant(fQuant),
                               m_fTimer(0.0f),
                               m_pCurrentRunningProcess(nullptr)
    {
        
    }
    
    void    MakeStep()
    {
        if(m_queProcesses.size() == 0)
        {
            m_eState = FINISHED;
        }
        
        while(m_fTimer <= m_fQuant &&
              m_queProcesses[m_CurrentRunningProcessIndex]->GetProcessState() != Process::FINISHED)
        {
            m_queProcesses[m_CurrentRunningProcessIndex]->MakeStep();
            m_fTimer += 1.0f;
        }
        
        if(m_queProcesses[m_CurrentRunningProcessIndex]->GetProcessState() == Process::FINISHED)
        {
            m_queProcesses.erase(m_queProcesses.begin() +
                                 m_CurrentRunningProcessIndex);
        }
        else
        {
            if(m_CurrentRunningProcessIndex == m_queProcesses.size()-1)
            {
                m_CurrentRunningProcessIndex = 0;
            }
        }
    }
    
    void    AddProcess(Process * pProcess)
    {
        if(m_queProcesses.size() == 0)
        {
            m_CurrentRunningProcessIndex = 0;
        }
        
        m_eState = RUNNING;
        m_queProcesses.push_back(pProcess);
    }
    
    ProcessManagerState GetState() const
    {
        return m_eState;
    }
    
protected:
    enum ProcessManagerState m_eState;
    const float              m_fQuant;
    float                    m_fTimer;
    size_t                   m_CurrentRunningProcessIndex;
    Process *                m_pCurrentRunningProcess;
    std::deque<Process*>     m_queProcesses;
};

class SPN_ProcessManager
{
public:
    SPN_ProcessManager() : m_pCurrentRunningProcess(nullptr)
    {
        
    }
    
    void    MakeStep()
    {
        if(m_pCurrentRunningProcess == nullptr)
        {
            m_pCurrentRunningProcess = m_queProcesses.front();
            if(m_pCurrentRunningProcess != nullptr)
            {
                for(auto& proc : m_queProcesses)
                {
                    if(proc->GetRemainingRunningTime() <
                       m_pCurrentRunningProcess->GetRemainingRunningTime())
                    {
                        m_pCurrentRunningProcess = proc;
                    }
                }
            }
        }
        
        if(m_pCurrentRunningProcess != nullptr)
        {
            m_pCurrentRunningProcess->MakeStep();
        }
        
        if(m_pCurrentRunningProcess->GetProcessState() == Process::FINISHED)
        {
            for(auto procIter = m_queProcesses.begin();
                procIter != m_queProcesses.end();
                ++procIter)
            {
                Process * pProc = *procIter;
                if(pProc->GetProcessState() == Process::FINISHED)
                {
                    m_queProcesses.erase(procIter);
                    break;
                }
            }
            
            m_pCurrentRunningProcess = nullptr;
        }
    }
    
    void    AddProcess(Process * pProcess)
    {
        if(m_pCurrentRunningProcess != nullptr)
        {
            if(m_pCurrentRunningProcess->GetRemainingRunningTime() >
               pProcess->GetRemainingRunningTime())
            {
                m_queProcesses.push_back(pProcess);
                m_pCurrentRunningProcess = m_queProcesses.back();
                return;
            }
        }
        
            // Stands for first process adding
        m_queProcesses.push_back(pProcess);
        m_pCurrentRunningProcess = m_queProcesses.front();
    }
    
    ProcessManagerState GetState() const
    {
        return m_eState;
    }
protected:
    Process *                m_pCurrentRunningProcess;
    enum ProcessManagerState m_eState;
    std::deque<Process*> m_queProcesses;
};

int main(int argc, const char * argv[])
{
    SPN_ProcessManager oProcessManager;
    RR_Manager oRRManager(4.0f);
    
    std::queue<std::pair<float, float>> queProcesses;
    queProcesses.push(std::make_pair(0.0f, 4.0f));
    queProcesses.push(std::make_pair(4.0f, 7.0f));
    queProcesses.push(std::make_pair(6.0f, 8.0f));
    queProcesses.push(std::make_pair(7.0f, 4.0f));
    queProcesses.push(std::make_pair(11.0f, 5.0f));
    queProcesses.push(std::make_pair(13.0f, 8.0f));
    queProcesses.push(std::make_pair(19.0f, 2.0f));
    queProcesses.push(std::make_pair(24.0f, 5.0f));
    
    while(queProcesses.size() != 0 &&
          oProcessManager.GetState() != ProcessManagerState::FINISHED)
    {
        if(queProcesses.size() != 0)
        {
            if(gCurrentTime == queProcesses.front().first)
            {
                Process * newProcess = new Process;
                newProcess->SetRemainingRunningTime(queProcesses.front().second);
//                oProcessManager.AddProcess(newProcess);
                oRRManager.AddProcess(newProcess);
                
                queProcesses.pop();
            }
        }
        
        oRRManager.MakeStep();
//        oProcessManager.MakeStep();
        
        gCurrentTime += 1.0f;
    }
    
    std::cout << "Done in " << gCurrentTime;
}
