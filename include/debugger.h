#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <string>
#include <vector>
#include "breakpoint.h"
namespace ccy{

class Debugger{
public:
    Debugger(std::string prog_name, pid_t pid):
            m_prog_name{prog_name}, m_pid{pid}{}

    void run();
    void handleCommand(const std::string& line);
    std::vector<std::string> split(const std::string&s, char delimiter);
    void continueExection();
    bool isPrefix(const std::string&s, const std::string& of);
    void setBreakpointAtAddress(std::intptr_t addr);
private:
    std::string m_prog_name;
    pid_t m_pid;
    std::unordered_map<std::intptr_t, std::unique_ptr<Breakpoint>> m_breakpoint;
};


}
#endif