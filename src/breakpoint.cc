#include <iostream>
#include <sys/ptrace.h>
#include <memory>
#include "breakpoint.h"
namespace ccy
{

void Breakpoint::enable(){
    auto data = ptrace(PTRACE_PEEKDATA, m_pid, m_addr, nullptr);
    m_saved_data = static_cast<uint8_t>(data & 0xff);
    uint64_t int3 = 0xcc;
    uint64_t data_int3 = ((data & ~0xff) | int3);

    ptrace(PTRACE_POKEDATA, m_pid, m_addr, data_int3);

    m_enabled = true;
}
void Breakpoint::disable(){
    auto data = ptrace(PTRACE_PEEKDATA, m_pid, m_addr, nullptr);
    auto restoredData = ((data & ~0xff) | m_saved_data);
    ptrace(PTRACE_POKEDATA, m_pid, m_addr, restoredData);

    m_enabled = false;

}


}