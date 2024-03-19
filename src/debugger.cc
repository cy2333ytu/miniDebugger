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
        m_prog_name{prog_name}, m_pid{pid}, m_memory{pid}
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

void Debugger::initializeLoadAddress() {
    if (m_pElf.get_hdr().type == elf::et::dyn) {
        std::ifstream map;
        std::string map_entry;
        std::stringstream ss;

        // 打开maps文件
        map.open("/proc/" + std::to_string(m_pid) + "/maps");
        if (map.is_open()) {
            // 读取第一行
            if (std::getline(map, map_entry)) {
                size_t dash_pos = map_entry.find('-');
                if (dash_pos != std::string::npos) {
                    std::string addr_str = map_entry.substr(0, dash_pos);
                    // 使用stringstream来进行转换
                    ss << std::hex << addr_str;
                    // 用合适的类型来处理64位地址
                    ss >> m_loadAddress;
                } else {
                    // 处理maps文件格式异常
                    std::cerr << "Error: Unexpected format in /proc/[pid]/maps." << std::endl;
                }
            } else {
                // 处理读取失败
                std::cerr << "Error: Unable to read /proc/[pid]/maps." << std::endl;
            }
            // 关闭文件
            map.close();
        } else {
            // 处理文件打开失败
            std::cerr << "Error: Unable to open /proc/[pid]/maps." << std::endl;
        }
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
    }else if(args[1].find(':') != std::string::npos){
        auto fileAndLine = split(args[1], ':');
        
    }else if(isPrefix(command, "memory")){
        std::string address{args[2], 2};
        if(isPrefix(args[1], "read")){
            
        }
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
dwarf::line_table::iterator Debugger::getLineEntryFromPc(uint64_t pc){
    for(auto& compilationUnit: m_pDwarf.compilation_units()){
        if(dwarf::die_pc_range(compilationUnit.root()).contains(pc)){
            auto It = compilationUnit.get_line_table();
            auto it = It.find_address(pc);
            if(it == It.end()){
                throw std::out_of_range{"Cannot find line entry"};
            }else{
                return it;
            }
        }
    }
    throw std::out_of_range("Cannot find line entry");
}

uint64_t Debugger::offsetLoadAddress(uint64_t addr){
    return addr - m_loadAddress;
}

siginfo_t Debugger::getSignalInfo(){
    siginfo_t info;
    ptrace(PTRACE_GETSIGINFO, m_pid, nullptr, &info);
    return info;
}

void Debugger::handleSignalTrap(siginfo_t info){
    switch(info.si_code){
        case SI_KERNEL:
        case TRAP_BRANCH: {
            // put the pc back where it should be
            m_memory.setPC(m_memory.getPC()-1);
            std::cout << "Hit breakpoint at address 0x" << std::hex << m_memory.getPC() << std::endl;
            auto offsetPC = offsetLoadAddress(m_memory.getPC());
            auto lineEntry = getLineEntryFromPc(offsetPC);
            printSource(lineEntry->file->path, lineEntry->line);
            return;
        }
        case TRAP_TRACE:
            return;
        default:
            std::cout << "Unknown SIGTRAP code " << info.si_code << std::endl;
            return;
    }
}

void Debugger::waitForSignal(){
    int waitStatus;
    auto options = 0;
    waitpid(m_pid, &waitStatus, options);
    auto siginfo = getSignalInfo();

    switch(siginfo.si_signo){
        case SIGTRAP:
            handleSignalTrap(siginfo);
            break; 
        case SIGSEGV:
            std::cout << "segfault. Reason: " << siginfo.si_code << std::endl;
            break;
        default:
            std::cout << "Got signal " << strsignal(siginfo.si_signo) << std::endl;
        
    }
}

void Debugger::printSource(const std::string& fileName, unsigned line, unsigned nLinesContext){
    std::ifstream file{fileName};
    if (!file.is_open()) {
        std::cerr << "Could not open the file: " << fileName << std::endl;
        return;
    }
    
    auto startLine = line <= nLinesContext ? 1 : line - nLinesContext;
    auto endLine = line + nLinesContext + (line < nLinesContext ? nLinesContext - line : 0) + 1;

    char c{};
    unsigned int currentLine = 1;
    while(currentLine != startLine && file.get(c)){
        if(c == '\n'){
            ++currentLine;
        }
    }
    std::cout << (currentLine == line ? "> " : "  ");
    
    while(currentLine <= endLine && file.get(c)){
        std::cout << c;
        if(c == '\n'){
            ++currentLine;
            std::cout << (currentLine == line ? "> " : "  ");
        }
    }
    //Write newline and make sure that the stream is flushed properly
    std::cout << std::endl;
    file.close();
}
void Debugger::stepOverBreakpoint(){
    if(m_breakpoint.count(m_memory.getPC())){
        auto& bp = m_breakpoint[m_memory.getPC()];
        if(bp->isEnable()){
            bp->disable();
            ptrace(PTRACE_SINGLESTEP, m_pid, nullptr, nullptr);
            waitForSignal();
            bp->enable();
        }
    }
}

void Debugger::singleStepInstruction(){
    ptrace(PTRACE_SINGLESTEP, m_pid, nullptr, nullptr);
    waitForSignal();
}

void Debugger::singleInstrucWithBpCheck(){
    if(m_breakpoint.count(m_memory.getPC())){
        stepOverBreakpoint();
    }else{
        singleStepInstruction();
    }
}

void Debugger::stepOut(){
    auto framePointer = m_memory.getRegisterValue(m_pid, Reg::rbp);
    auto returnAddress = m_memory.readMemory(framePointer+8);
    bool shouldRemoveBp = false;
    if(!m_breakpoint.count(returnAddress)){
        setBreakpointAtAddress(returnAddress);
        shouldRemoveBp = true;
    }
    continueExection();
    
    if(shouldRemoveBp){
        removeBreakpoint(returnAddress);
    }
}

void Debugger::removeBreakpoint(std::intptr_t address){
    if(m_breakpoint.at(address)->isEnable()){
        m_breakpoint.at(address)->disable();
    }
    m_breakpoint.erase(address);
}

}