#include <iostream>
#include <string>
#include <stdlib.h>
#define CAP 8192000
#define ERR -500
//#define CAP 16384000
#define MAX_FREQ_LIMIT_THR 2700000
#define MAX_FREQ_LIMIT_PSAVE 1200000
#define PREVENT_LOW_PERF 1500000
string dtoitoa(float input, char *) {
	int n = input+0.5;
	int i,sign;
	if((sign=n)<0) {
	    n=-n;
	}
	i = 0;
	do {
	    s[i++] = n%10+'0';
	} while ((n/=10)>0);
	if(sign<0) {
	    s[i++]='-';
	}
	reverse(s);
}
namespace MemoryUtils {
    const std::string tempLogFile = "/tmp/.cMem/.meminfo.log";
    const std::string tempConfigurationFile = "/tmp/.cMem/.temp_sysctl.log";
    const std::string governorWhenMemSuperGood = "powersave";
    const std::string governorWhenMemGood = "schedutil";
    const std::string governorWhenMemBad = "conservative";
    const std::string governorWhenMemSuperBad = "performance";
    int *memStat = NULL;
    fstream tempFileStream;
    fstream tempLogFileStream;
    void readMemStatus(string Token) {
	static int statOrigin = ERR;
        system("cat /proc/meminfo | grep "+Token+" | awk \"{print $2}\" > /tmp/.cMem/.meminfo.log");
        tempLogFileStream.open(tempLogFile, "rw");
	tempLogFileStream >> statOrigin;
	memStat = &statOrigin;

    }

    void setCPUGovernor(void) {
        system("cpupower frequency-set -d 0 -u 1000000");
        if(*memStat<CAP*0.15) {
            system("cpupower frequency-set -g "+governorWhenMemSuperBad);
            system("cpupower frequency-set -d "+dtoitoa(PREVENT_LOW_PERF));
        } else if(*memStat<CAP*0.3) {
            system("cpupower frequency-set -g "+governorWhenMemBad + " -u "+dtoitoa(MAX_FREQ_LIMIT_THR));
            system("cpupower frequency-set -d "+dtoitoa(PREVENT_LOW_PERF*0.9));
        } else if(*memStat<CAP*0.5) {
            system("cpupower frequency-set -g "+governorWhenMemGood);
        } else {
            system("cpupower frequency-set -u "+dtoitoa(MAX_FREQ_LIMIT_PSAVE));
        }
    }
    void setVMSwappiness (void) {
        tempFileStream.open(tempConfigurationFile, "rw"); /*Configuration file, inside of its temp folder*/
        tempFileDescriptor << "vm.swappiness="+(*memStat/CAP)/*Left Memory Space*/*100.f/*By percent*/;
        system("sync");
        system("sysctl --load -w -p "+tempConfigurationFile);
    }
    void setMemMonitor(string metric) {
	readMemStatus(metric);
        setVMSwappiness(*memStat);
	setCPUGovernor(*memStat);
    }
}
