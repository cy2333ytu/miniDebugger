#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <string>

namespace ccy{

class Debugger{
public:
    Debugger(std::string prog_name, pid_t pid):
            m_prog_name{prog_name}, m_pid{pid}{}

    void run();
private:
    std::string m_prog_name;
    pid_t m_pid;
};


}
#endif