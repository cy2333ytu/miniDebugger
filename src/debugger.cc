#include<sys/types.h>
#include<sys/wait.h>

#include"linenoise.h"
#include "debugger.h"
namespace ccy
{

void Debugger::run(){
    int wait_status;
    auto options = 0;
    waitpid(m_pid, &wait_status, options);

    char *line = nullptr;
    while((line == linenoise("minidbg> ")) != nullptr){
        handleCommand(line);
        linenoiseHistoryAdd(line);
        linenoiseFree(line);
    }
}
}