//
//  main.cpp
//  OS_L3
//
//  Created by Aleksandr Borzikh on 28.11.16.
//  Copyright © 2016 sandyre. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include <dispatch/dispatch.h>

    // Synchonizes stdout between threads
std::mutex g_STDOUTMutex;
void ThreadSafePrint(const char * pText)
{
    g_STDOUTMutex.lock();
    std::cout << pText;
    g_STDOUTMutex.unlock();
}

class WorkThread
{
public:
    WorkThread() = delete;
    
    WorkThread(const std::vector<float>& g_Data,
               std::vector<float>& g_ResData,
               dispatch_semaphore_t g_BinSem,
               std::recursive_mutex& g_CritSection) :
    m_Data(g_Data),
    m_ResData(g_ResData),
    m_BinSemaphore(g_BinSem),
    m_fSleepTime(0.0f),
    m_CritSection(g_CritSection)
    {
    }
    
    void Start()
    {
        m_oThread = std::thread(&WorkThread::Work, this);
        m_oThread.detach();
    }
    
private:
    void Work()
    {
        m_CritSection.lock();
        ThreadSafePrint("Worker: Enter sleep interval\n");
        std::cin >> m_fSleepTime;
        ThreadSafePrint("Worker: Fine, lets do some work!\n");
        
        std::vector< std::pair< int, int > > v_pairs;
        v_pairs.push_back(std::make_pair(m_Data[0], 1));
        
        for(size_t i = 1; i < m_Data.size(); ++i)
        {
            for(size_t j = 0; j < v_pairs.size(); ++j)
            {
                if(v_pairs[j].first == m_Data[i])
                {
                    ++v_pairs[j].second;
                    break;
                }
                if(j == v_pairs.size()-1)
                {
                    v_pairs.push_back(std::make_pair(m_Data[i], 1));
                    break;
                }
            }
        }
        
        std::sort(v_pairs.begin(), v_pairs.end(), [&](auto a, auto b)
                  {
                      return a.second > b.second;
                  });
        
            // lets go
        for(size_t i = 0; i < v_pairs.size(); ++i)
        {
            for(size_t j = 0; j < v_pairs[i].second; ++j)
            {
                dispatch_semaphore_wait(m_BinSemaphore, DISPATCH_TIME_FOREVER);
                m_ResData.push_back(v_pairs[i].first);
                dispatch_semaphore_signal(m_BinSemaphore);
                std::this_thread::sleep_for(std::chrono::seconds((int)m_fSleepTime));
            }
        }
        ThreadSafePrint("Worker: Finished!\n");
        m_CritSection.unlock();
    }
    
    std::recursive_mutex& m_CritSection;
    dispatch_semaphore_t m_BinSemaphore;
    float m_fSleepTime;
    const std::vector<float>& m_Data;
    std::vector<float>& m_ResData;
    std::thread m_oThread;
};

class SumThread
{
public:
    SumThread() = delete;
    
    SumThread(const std::vector<float>& g_Data,
              std::vector<float>& g_ResData,
              std::recursive_mutex& g_CritSection) :
    m_Data(g_Data),
    m_ResData(g_ResData),
    m_CritSection(g_CritSection)
    {
    }
    
    void Start()
    {
        m_oThread = std::thread(&SumThread::Work, this);
        m_oThread.detach();
    }
    
private:
    void Work()
    {
        while(true)
        {
            m_CritSection.lock();
            if(m_ResData.size() == 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
            else
            {
                break;
            }
            m_CritSection.unlock();
        }
        float Sum = 0;
        for(size_t i = 0; i < m_ResData.size(); ++i)
        {
            Sum += m_ResData[i];
        }
        ThreadSafePrint("Summer: Sum of elements is: ");
        ThreadSafePrint(std::to_string(Sum).c_str());
        ThreadSafePrint("\n");
        ThreadSafePrint("Summer: Finished!\n");
        m_CritSection.unlock();
    }
    
    const std::vector<float>& m_Data;
    std::vector<float>& m_ResData;
    std::thread m_oThread;
    std::recursive_mutex& m_CritSection;
};

int main(int argc, const char * argv[])
{
    size_t nSize = 0;
    dispatch_semaphore_t g_BinSemaphore = dispatch_semaphore_create(5);
    std::recursive_mutex g_CritSection;
    std::vector<float> g_Data;
    std::vector<float> g_ResultedData;
    WorkThread oWorkThread(g_Data, g_ResultedData, g_BinSemaphore, g_CritSection);
    SumThread  oSumThread(g_Data, g_ResultedData, g_CritSection);
    
    ThreadSafePrint("Main: Enter elements count\n");
    std::cin >> nSize;
    g_Data.reserve(nSize);
    g_ResultedData.reserve(nSize);
    
    int dTmp = 0;
    ThreadSafePrint("Main: Enter elements 1 by 1\n");
    for(size_t i = 0; i < nSize; ++i)
    {
        std::cin >> dTmp;
        g_Data.push_back(dTmp);
    }
    
    std::stringstream str_stream;
    str_stream << "\nMain: You have entered " << nSize << "elements\n";
    ThreadSafePrint(str_stream.str().c_str());
    str_stream.str("");
    str_stream.clear();
    for(size_t i = 0; i < nSize; ++i)
    {
        str_stream << g_Data[i] << "\n";
        ThreadSafePrint(str_stream.str().c_str());
        str_stream.str("");
        str_stream.clear();
    }
        // myaso
    oWorkThread.Start();
    oSumThread.Start();
    
        // interesting part!
    size_t nPreviousSize = 0;
    while(true)
    {
        dispatch_semaphore_wait(g_BinSemaphore, DISPATCH_TIME_FOREVER);
        if(nPreviousSize != g_ResultedData.size())
        {
            nPreviousSize = g_ResultedData.size();
            for(size_t i = 0; i < nPreviousSize; ++i)
            {
                ThreadSafePrint(std::to_string(g_ResultedData[i]).c_str());
                ThreadSafePrint(" ");
            }
            ThreadSafePrint("\n");
        }
        dispatch_semaphore_signal(g_BinSemaphore);
    }
    
    dispatch_release(g_BinSemaphore);
    return 0;
}
