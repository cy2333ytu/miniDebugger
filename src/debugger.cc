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
    Debugger::Debugger(std::string prog_name, pid_t pid):
        m_prog_name{prog_name}, m_pid{pid}
    {
        m_prog_name = prog_name;
        m_pid = pid;
        auto fd = open(m_prog_name.c_str(), O_RDONLY);
        m_pElf = elf::elf{elf::create_mmap_loader(fd)};
        m_pDwarf = dwarf::dwarf{dwarf::elf::create_loader(m_pElf)};
        close(fd);
    }
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

void Debugger::setBreakpointAtAddress(std::intptr_t addr){
    std::cout << "Set breakpoint at address 0x" << std::hex << addr << std::endl;
    auto bp = std::make_unique<Breakpoint>(m_pid, addr);
    bp->enable();
    m_breakpoint[addr] = std::move(bp);
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

dwarf::die Debugger::getFunctionFromPc(uint64_t pc){
    for(auto& compilationUnit: m_pDwarf.compilation_units()){
        if(dwarf::die_pc_range(compilationUnit.root()).contains(pc)){
            for(auto& die : compilationUnit.root()){
                if(die.tag == dwarf::DW_TAG::subprogram){
                    if(dwarf::die_pc_range(die).contains(pc)){
                        return die;
                    }
                }
            }
        }
    }
    throw std::out_of_range{"Cannot find function"};

}

}