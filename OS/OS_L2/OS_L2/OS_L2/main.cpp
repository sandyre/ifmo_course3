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

float gTimer = 0.0f;

class Process
{
public:
    enum ProcessState
    {
        RUNNING = 0x1,
        FINISHED = 0x2
    };
    
    Process() : m_eState(RUNNING),
                m_fProcessStartingTime(0.0f),
                m_fRemainingRunningTime(0.0f)
    {
    }
    
    void MakeStep()
    {
        if(m_eState == RUNNING)
        {
            if(m_fRemainingRunningTime - 1.0f == 0)
            {
                m_fRemainingRunningTime -= 1.0f;
                m_eState = FINISHED;
                std::cout << "Process finished. It took " << gTimer - m_fProcessStartingTime;
                std::cout << std::endl;
            }
            else
            {
                m_fRemainingRunningTime -= 1.0f;
            }
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
    void SetProcessStartingTime(float fValue)
    {
        m_fProcessStartingTime = fValue;
    }
    
protected:
    enum ProcessState m_eState;
    float m_fProcessStartingTime;
    float m_fRemainingRunningTime;
};

class RR_Algorithm
{
public:
    RR_Algorithm(const float fQuant) : m_fQuant(fQuant),
                                       m_iCurrentlyRunningProcess(0)
    {
    }
    
    void MakeStep()
    {
        if(m_queProcesses.size() == 0)
            return;
        
        for(float i = 0.0f; i < m_fQuant; ++i)
        {
            m_queProcesses[m_iCurrentlyRunningProcess].MakeStep();
            if(m_queProcesses[m_iCurrentlyRunningProcess].GetProcessState() ==
               Process::FINISHED)
            {
                m_queProcesses.erase(m_queProcesses.begin() + m_iCurrentlyRunningProcess);
                if(m_iCurrentlyRunningProcess == m_queProcesses.size())
                    m_iCurrentlyRunningProcess = 0;
                else
                    ++m_iCurrentlyRunningProcess;
                return;
            }
        }
    }
    
    void AddProcess(Process oProc)
    {
        if(m_iCurrentlyRunningProcess == 0)
        {
            m_queProcesses.push_back(oProc);
        }
        else
        {
            m_queProcesses.insert(m_queProcesses.begin() + m_iCurrentlyRunningProcess - 1,
                                  oProc);
        }
    }
    
    size_t GetProcessesCount() const
    {
        return m_queProcesses.size();
    }
    
protected:
    size_t m_iCurrentlyRunningProcess;
    std::deque< Process > m_queProcesses;
    const float m_fQuant;
};

class SPN_Algorithm
{
public:
    SPN_Algorithm()
    {
    }
    
    void MakeStep()
    {
        if(m_queProcesses.size() == 0)
            return;
        
        m_queProcesses.front().MakeStep();
        if(m_queProcesses.front().GetProcessState() ==
           Process::FINISHED)
        {
            m_queProcesses.pop_front();
            return;
        }
    }
    
    void AddProcess(Process oProc)
    {
        size_t iInsertionIndex = 0;
        for(auto i = 0; i < m_queProcesses.size(); ++i)
        {
            if(m_queProcesses[i].GetRemainingRunningTime() <= oProc.GetRemainingRunningTime())
                ++iInsertionIndex;
        }
        m_queProcesses.insert(m_queProcesses.begin() + iInsertionIndex,
                              oProc);
    }
    
    size_t GetProcessesCount() const
    {
        return m_queProcesses.size();
    }
    
protected:
    std::deque< Process > m_queProcesses;
};

int main(int argc, const char * argv[])
{
    SPN_Algorithm oSPNManager;
    RR_Algorithm oRRManager(4.0f);
    
    std::queue<std::pair<float, float>> queProcesses;
    queProcesses.push(std::make_pair(0.0f, 4.0f));
    queProcesses.push(std::make_pair(4.0f, 7.0f));
    queProcesses.push(std::make_pair(6.0f, 8.0f));
    queProcesses.push(std::make_pair(7.0f, 4.0f));
    queProcesses.push(std::make_pair(11.0f, 5.0f));
    queProcesses.push(std::make_pair(13.0f, 8.0f));
    queProcesses.push(std::make_pair(19.0f, 2.0f));
    queProcesses.push(std::make_pair(24.0f, 5.0f));
    
    while(true)
    {
        if(queProcesses.size() != 0)
        {
            if(gTimer == queProcesses.front().first)
            {
                Process oProc;
                oProc.SetProcessStartingTime(queProcesses.front().first);
                oProc.SetRemainingRunningTime(queProcesses.front().second);
                oRRManager.AddProcess(oProc);
                oSPNManager.AddProcess(oProc);
                
                queProcesses.pop();
            }
        }
        
        gTimer += 1.0f;
        
        oRRManager.MakeStep();
        oSPNManager.MakeStep();
        
        if(queProcesses.size() == 0 &&
           oRRManager.GetProcessesCount() == 0 &&
           oSPNManager.GetProcessesCount() == 0)
            break;
    }
    
    std::cout << "Done in " << gTimer << std::endl;
    
    return 0;
}
