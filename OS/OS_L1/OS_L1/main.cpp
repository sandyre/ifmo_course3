#include <stdio.h>
#include <unistd.h>
#include <mach/mach.h>
#include <sys/types.h>
#include <sys/sysctl.h>

int main(int argc, const char * argv[])
{
    size_t nSize = 0;
    int  nVMStatSize = HOST_VM_INFO_COUNT;
    vm_statistics_data_t stVMData;
    char pszCPUName[255];
    char pszKernelVersion[255];
    char pszComputerName[255];
    char pszUsername[255];
    
    nSize = sizeof(pszCPUName);
    sysctlbyname("machdep.cpu.brand_string",
                 pszCPUName,
                 &nSize,
                 NULL, 0);
    
    host_statistics(mach_host_self(), HOST_VM_INFO,
                    (integer_t*)&stVMData,
                    (mach_msg_type_number_t*)&nVMStatSize);
    
    nSize = sizeof(pszKernelVersion);
    sysctlbyname("kern.version",
                 pszKernelVersion,
                 &nSize,
                 NULL, 0);
    
    
    nSize = sizeof(pszComputerName);
    sysctlbyname("kern.hostname",
                 pszComputerName,
                 &nSize,
                 NULL, 0);
    
    nSize = sizeof(pszUsername);
    getlogin_r(pszUsername, nSize);
    
    printf("\t\tSystemInfo\n"
           "\tOSX Kernel version: %s\n"
           "\tComputer name: %s\n"
           "\tUser name: %s\n"
           "\tCPU Info: %s\n"
           "\tFree memory pages count: %u\n",
           pszKernelVersion,
           pszComputerName,
           pszUsername,
           pszCPUName,
           stVMData.free_count);
}
