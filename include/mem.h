#ifndef MEM_H
#define MEM_H
#include"reg.h"
#include<cstdint>

namespace ccy
{

class Memory{
private:
    pid_t m_pid;
public:
    Memory(pid_t p): m_pid{p} {}
    
    uint64_t getRegisterValue(pid_t pid,  Reg r);
    void setRegisterValue(pid_t pid, Reg r, uint64_t value);

    uint64_t getRegisterValueFromDwarfRegister(pid_t pid, unsigned regNum);

    std::string getRegisterName(Reg r);
    Reg getRegisterFromName(const std::string& name);
    void dumpRegisters();

    uint64_t readMemory(uint64_t address);
    void writeMemory(uint64_t address, uint64_t value);

    uint64_t getPC();
    void setPC(uint64_t pc);
};

}

#endif