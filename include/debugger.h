#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "breakpoint.h"
#include "elf/elf++.hh"
#include "dwarf/dwarf++.hh"
#include "mem.h"
#include "signal.h"

#include <string>
#include <vector>

namespace ccy{

class Debugger{
public:
    Debugger(std::string prog_name, pid_t pid);

    void run();
    void waitForSignal();
    void initializeLoadAddress();
    uint64_t offsetLoadAddress(uint64_t addr);
    siginfo_t getSignalInfo();

    void handleCommand(const std::string& line);
    std::vector<std::string> split(const std::string&s, char delimiter);
    void continueExection();
    bool isPrefix(const std::string&s, const std::string& of);
    void setBreakpointAtAddress(std::intptr_t addr);
    dwarf::die getFunctionFromPc(uint64_t pc);
    dwarf::line_table::iterator getLineEntryFromPc(uint64_t pc);

    void handleSignalTrap(siginfo_t info);
    void printSource(const std::string& fileName, unsigned line, unsigned nLinesContext = 2);
    void stepOverBreakpoint();
    void singleStepInstruction();

    void singleInstrucWithBpCheck();
    void stepOut();
    void removeBreakpoint(std::intptr_t);
private:
    std::string m_prog_name;
    pid_t m_pid;
    std::unordered_map<std::intptr_t, std::unique_ptr<Breakpoint>> m_breakpoint;
    elf::elf m_pElf;          // Executable and Linkable Format
    dwarf::dwarf m_pDwarf;    // Debugging With Arbitrary Record Format
    uint64_t m_loadAddress = 0;
    Memory m_memory;
};


}
#endif