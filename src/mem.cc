#include "mem.h"
#include "sys/ptrace.h"
#include <algorithm>
#include <iostream>
#include <iomanip>

namespace ccy{

    uint64_t Memory::getRegisterValue(pid_t pid,  Reg r){
        user_regs_struct regs;
        ptrace(PTRACE_GETREGS, pid, nullptr, &regs);
        auto it = std::find_if(std::begin(Registers), std::end(Registers), 
            [r](auto&& rd){return rd.reg == r;});
        return *(reinterpret_cast<uint64_t*>(&regs) + (it - std::begin(Registers)));
    }
    void Memory::setRegisterValue(pid_t pid, Reg r, uint64_t value){
        user_regs_struct regs;
        ptrace(PTRACE_GETREGS, pid, nullptr, &regs);
        auto it = std::find_if(std::begin(Registers), std::end(Registers), 
            [r](auto&& rd){return rd.reg == r;});
        *(reinterpret_cast<uint64_t*>(&regs) + (it - std::begin(Registers))) = value;
        ptrace(PTRACE_SETREGS, pid, nullptr, &regs);
    }

    uint64_t Memory::getRegisterValueFromDwarfRegister(pid_t pid, unsigned regNum){
        auto it = std::find_if(std::begin(Registers), std::end(Registers), 
            [regNum](auto && rd){return rd.dwarfReg == regNum;});
        if(it == std::end(Registers)){
            throw std::out_of_range("Unknown dwarf register");
        }
        return getRegisterValue(pid, it->reg);
    }

    std::string Memory::getRegisterName(Reg r){
        auto it = std::find_if(std::begin(Registers), std::end(Registers),
            [r](auto && rd){return rd.reg == r;});
        return it->name;
    }
    Reg Memory::getRegisterFromName(const std::string& name){
        auto it = std::find_if(std::begin(Registers), std::end(Registers),
            [name](auto && rd){return rd.name == name;});
        return it->reg;
    }
    void Memory::dumpRegisters(){
        for(const auto& rd: Registers){
            std::cout << rd.name << " 0x"
            << std::setfill('0') << std::setw(16) << std::hex << getRegisterValue(m_pid, rd.reg) << std::endl;
        }
    }

    uint64_t Memory::readMemory(uint64_t address){return ptrace(PTRACE_PEEKDATA, m_pid, address, nullptr);}
    void Memory::writeMemory(uint64_t address, uint64_t value){ ptrace(PTRACE_POKEDATA, m_pid, address, value);}

    uint64_t Memory::getPC(){return getRegisterValue(m_pid, Reg::rip);}
    void Memory::setPC(uint64_t pc){setRegisterValue(m_pid, Reg::rip, pc);}

}