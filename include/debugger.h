#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <string>
#include <vector>

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
private:
    std::string m_prog_name;
    pid_t m_pid;
};


}
#endif