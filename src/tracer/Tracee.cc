#include "Tracee.h"

#include "common/Exception.h"
#include "common/WithErrnoCheck.h"

#include <sys/ptrace.h>
#include <signal.h>

namespace s2j {
namespace tracer {

Tracee::Tracee(pid_t traceePid)
    : traceePid_(traceePid)
    , syscallArch_(Arch::UNKNOWN) {
    if (isAlive()) {
        try {
            withErrnoCheck("ptrace getregs", ptrace, PTRACE_GETREGS, traceePid_, nullptr, &regs_);
        }
        catch (const SystemException& e) {
            if (e.getErrno() != ESRCH)
                throw;
        }
    }
}

bool Tracee::isAlive() {
    return kill(traceePid_, 0) == 0;
}

int64_t Tracee::getEventMsg() {
    int64_t code;
    withErrnoCheck("ptrace geteventmsg", ptrace, PTRACE_GETEVENTMSG, traceePid_, nullptr, &code);
    return code;
}

void Tracee::setSyscallArch(Arch arch) {
    syscallArch_ = arch;
}

Arch Tracee::getSyscallArch() const {
    return syscallArch_;
}

uint64_t Tracee::getSyscallNumber() {
    if (syscallArch_ == Arch::UNKNOWN)
        throw Exception("Can't get syscall number, unknown syscall arch");
    return regs_.orig_rax;
}

uint64_t Tracee::getSyscallArgument(uint8_t argumentNumber) {
    if (syscallArch_ == Arch::X86) {
        if (argumentNumber == 0)
            return static_cast<uint32_t>(regs_.rbx);
        else if (argumentNumber == 1)
            return static_cast<uint32_t>(regs_.rcx);
        else if (argumentNumber == 2)
            return static_cast<uint32_t>(regs_.rdx);
        else if (argumentNumber == 3)
            return static_cast<uint32_t>(regs_.rsi);
        else if (argumentNumber == 4)
            return static_cast<uint32_t>(regs_.rdi);
        else if (argumentNumber == 5)
            return static_cast<uint32_t>(regs_.rbp);
    }
    else if (syscallArch_ == Arch::X86_64) {
        if (argumentNumber == 0)
            return regs_.rdi;
        else if (argumentNumber == 1)
            return regs_.rsi;
        else if (argumentNumber == 2)
            return regs_.rdx;
        else if (argumentNumber == 3)
            return regs_.r10;
        else if (argumentNumber == 4)
            return regs_.r8;
        else if (argumentNumber == 5)
            return regs_.r9;
    }
    else {
        throw Exception("Can't get syscall argument, unknown syscall arch");
    }
    throw Exception("No such syscall argument number " + std::to_string(argumentNumber));
}

std::string Tracee::getMemoryString(uint64_t address, size_t sizeLimit) {
    NOT_IMPLEMENTED();
    // This code is broken in may ways. Implement using process_vm_readv syscall.
    /*
    for (size_t ptr = address; str.size() < sizeLimit; ptr += sizeof(int)) {
        int word = withErrnoCheck("ptrace peektext", ptrace, PTRACE_PEEKTEXT, traceePid_, ptr, nullptr);
        for (size_t i = 0; i < sizeof(word); ++i) {
            char byte = (word >> (8 * i));
            if (byte == '\0')
                return str;
            str += byte;
        }
    }
    */
}

void Tracee::cancelSyscall(uint64_t returnValue) {
    regs_.orig_rax = -1;
    regs_.rax = returnValue;
    withErrnoCheck("ptrace setregs", ptrace, PTRACE_SETREGS, traceePid_, nullptr, &regs_);
}

}

std::string to_string(const tracer::Arch arch) {
    switch (arch) {
        case tracer::Arch::X86:
            return "x86";
        case tracer::Arch::X86_64:
            return "x86_64";
        default:
            return "UNKNOWN";
    }
}

}
