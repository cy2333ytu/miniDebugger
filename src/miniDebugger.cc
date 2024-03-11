#include <iostream>
#include <sys/ptrace.h>
#include <unistd.h>

#include"debugger.h"

int main(int argc, char* argv[]){
    if(argc < 2){
        std::cerr << "Program name not specified";
        return -1;
    }
    auto progName = argv[1];
    auto pid = fork();

    if(pid == 0){
        // in child process
    }else if(pid >= 1){
        // in parent process
        std::cout << "Start debugger process " << pid << "\n";
        ccy::Debugger dbg(progName, pid);
        dbg.run();    
    }
}