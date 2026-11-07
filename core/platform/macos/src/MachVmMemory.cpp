#include <platform/macos/MachVmMemory.hpp>

#ifdef __APPLE__
#include <mach/mach.h>
#include <mach/mach_vm.h>
#endif

namespace Platform::MacOs {

bool MachVmMemory::Allocate(std::uintptr_t& address, std::size_t size, bool anywhere) {
#ifdef __APPLE__
    mach_vm_address_t targetAddr = address;
    const int flags = anywhere ? VM_FLAGS_ANYWHERE : VM_FLAGS_FIXED;
    const kern_return_t kr = mach_vm_allocate(mach_task_self(), &targetAddr, size, flags);
    if (kr == KERN_SUCCESS) {
        address = static_cast<std::uintptr_t>(targetAddr);
        return true;
    }
    return false;
#else
    (void)address;
    (void)size;
    (void)anywhere;
    return false;
#endif
}

bool MachVmMemory::Deallocate(std::uintptr_t address, std::size_t size) {
#ifdef __APPLE__
    const kern_return_t kr = mach_vm_deallocate(mach_task_self(), static_cast<mach_vm_address_t>(address), size);
    return kr == KERN_SUCCESS;
#else
    (void)address;
    (void)size;
    return false;
#endif
}

bool MachVmMemory::Remap(std::uintptr_t& targetAddress, std::size_t size, std::uintptr_t sourceAddress, bool copyOnWrite) {
#ifdef __APPLE__
    mach_vm_address_t remapAddr = targetAddress;
    vm_prot_t curProt = 0;
    vm_prot_t maxProt = 0;
    const int flags = (targetAddress == 0) ? VM_FLAGS_ANYWHERE : (VM_FLAGS_FIXED | VM_FLAGS_OVERWRITE);
    const boolean_t copy = copyOnWrite ? TRUE : FALSE;
    const kern_return_t kr = mach_vm_remap(
        mach_task_self(),
        &remapAddr,
        size,
        0,
        flags,
        mach_task_self(),
        static_cast<mach_vm_address_t>(sourceAddress),
        copy,
        &curProt,
        &maxProt,
        VM_INHERIT_NONE
    );
    if (kr == KERN_SUCCESS) {
        targetAddress = static_cast<std::uintptr_t>(remapAddr);
        return true;
    }
    return false;
#else
    (void)targetAddress;
    (void)size;
    (void)sourceAddress;
    (void)copyOnWrite;
    return false;
#endif
}

bool MachVmMemory::Protect(std::uintptr_t address, std::size_t size, bool readable, bool writable, bool executable) {
#ifdef __APPLE__
    vm_prot_t prot = VM_PROT_NONE;
    if (readable) prot |= VM_PROT_READ;
    if (writable) prot |= VM_PROT_WRITE;
    if (executable) prot |= VM_PROT_EXECUTE;
    const kern_return_t kr = mach_vm_protect(mach_task_self(), static_cast<mach_vm_address_t>(address), size, FALSE, prot);
    return kr == KERN_SUCCESS;
#else
    (void)address;
    (void)size;
    (void)readable;
    (void)writable;
    (void)executable;
    return false;
#endif
}

} // namespace Platform::MacOs
