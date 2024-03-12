#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <fcntl.h>
#include <stdexcept>
#include <sys/ptrace.h>

#include "linenoise.h"
#include "debugger.h"
#include "breakpoint.h"

namespace ccy
{

void Debugger::run(){
    int wait_status;
    auto options = 0;
    waitpid(m_pid, &wait_status, options);

    char *line = nullptr;
    while((line = linenoise("minidbg> ")) != nullptr){
        handleCommand(line);
        linenoiseHistoryAdd(line);
        linenoiseFree(line);
    }
}

void Debugger::handleCommand(const std::string& line){
    auto args = split(line, ' ');
    auto command = args[0];
    if(isPrefix(command, "cont")){
        continueExection();
    }else if(isPrefix(command, "break")){
        std::string addr{args[1], 2};
        setBreakpointAtAddress(std::stol(addr, 0, 16));
    }
    else{
        std::cerr << "Unknow command\n";
    }
}

std::vector<std::string> Debugger::split(const std::string&s, char delimiter){
    std::vector<std::string> res{};
    std::stringstream ss{s};
    std::string item;
    while(std::getline(ss, item, delimiter)){
        res.emplace_back(item);
    }
    return res;
}

bool Debugger::isPrefix(const std::string&s, const std::string& of){
    if(s.size() > of.size()) return false;
    return std::equal(s.begin(), s.end(), of.begin());
}

void Debugger::continueExection(){
    ptrace(PTRACE_CONT, m_pid, nullptr, nullptr);
    int wait_status;
    auto options = 0;
    waitpid(m_pid, &wait_status, options);
}

}