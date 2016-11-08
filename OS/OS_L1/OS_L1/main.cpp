#include <stdio.h>
#include <sys/types.h>
#include <sys/sysctl.h>

int main(int argc, const char * argv[])
{
    size_t   uint64Size         = sizeof(uint64_t);
    size_t   charPointerSize    = sizeof(char*);
    uint64_t nCPUFrequency      = 0;
    sysctlbyname("hw.cpufrequency",
                 &nCPUFrequency,
                 &uint64Size, NULL, 0);
    
    uint64_t nLogicalCPUCount = 0;
    sysctlbyname("hw.logicalcpu",
                 &nLogicalCPUCount,
                 &uint64Size, NULL, 0);
    
    char pszKernelType[20];
    sysctlbyname("kern.ostype",
                 pszKernelType,
                 &charPointerSize,
                 NULL, 0);
    
    char pszKernelVersion[30];
    sysctlbyname("kern.osrelease",
                 pszKernelVersion,
                 &charPointerSize,
                 NULL, 0);
    
    printf("\t\tSystem info:\n");
}
