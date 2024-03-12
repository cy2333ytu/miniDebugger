#ifndef BREAKPOINT_H
#define BREAKPOINT_H
#include <sys/types.h>
#include <iostream>
#include <memory>
#include <unordered_map>
namespace ccy
{

class Breakpoint{
public:
    Breakpoint(pid_t pid, std::intptr_t addr)
        : m_pid{pid}, m_addr{addr}, m_enabled{false}, m_saved_data{}{}
    void enable();
    void disable();
    bool isEnable() const {return m_enabled;}
    std::intptr_t getAddr() const {return m_addr;}
    void setBreakpointAtAddress(std::intptr_t addr);
private:
    pid_t m_pid;
    std::intptr_t m_addr;
    bool m_enabled;
    uint8_t m_saved_data;
    std::unordered_map<std::intptr_t, std::unique_ptr<Breakpoint>> m_breakpoint;
};


}
#endif