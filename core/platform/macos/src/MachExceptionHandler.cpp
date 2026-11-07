#include <platform/macos/MachExceptionHandler.hpp>

#ifdef __APPLE__
#include <atomic>
#include <mach/mach.h>
#include <mach/mach_vm.h>
#include <pthread.h>
#include <unistd.h>

extern "C" {
boolean_t mach_exc_server(mach_msg_header_t* in, mach_msg_header_t* out);

kern_return_t catch_mach_exception_raise(
    mach_port_t,
    mach_port_t thread,
    mach_port_t task,
    exception_type_t exception,
    mach_exception_data_t code,
    mach_msg_type_number_t codeCount
) {
    if (exception == EXC_BAD_ACCESS && codeCount >= 2) {
        auto& handler = Platform::MacOs::MachExceptionHandler::Instance();
        const auto faultAddr = static_cast<std::uintptr_t>(code[1]);
        const bool isWrite = (code[0] == 2);
        if (handler.HandleBadAccess(faultAddr, isWrite)) {
            mach_port_deallocate(mach_task_self(), thread);
            mach_port_deallocate(mach_task_self(), task);
            return KERN_SUCCESS;
        }
    }
    mach_port_deallocate(mach_task_self(), thread);
    mach_port_deallocate(mach_task_self(), task);
    return KERN_FAILURE;
}

kern_return_t catch_mach_exception_raise_state(mach_port_t, exception_type_t, const mach_exception_data_t, mach_msg_type_number_t, int*, const thread_state_t, mach_msg_type_number_t, thread_state_t, mach_msg_type_number_t*) {
    return KERN_FAILURE;
}

kern_return_t catch_mach_exception_raise_state_identity(mach_port_t, mach_port_t, mach_port_t, exception_type_t, mach_exception_data_t, mach_msg_type_number_t, int*, thread_state_t, mach_msg_type_number_t, thread_state_t, mach_msg_type_number_t*) {
    return KERN_FAILURE;
}
}
#endif

namespace Platform::MacOs {

struct MachExceptionHandler::Impl {
#ifdef __APPLE__
    mach_port_t excPort = MACH_PORT_NULL;
    pthread_t listenerThread{};
    std::atomic<bool> running{false};
    BadAccessCallback callback;

    static void* ServerLoop(void* arg) {
        auto* self = static_cast<Impl*>(arg);
        char req[1024];
        char rep[1024];
        auto* in = reinterpret_cast<mach_msg_header_t*>(req);
        auto* out = reinterpret_cast<mach_msg_header_t*>(rep);

        while (self->running.load(std::memory_order_relaxed)) {
            const kern_return_t kr = mach_msg(
                in,
                MACH_RCV_MSG | MACH_RCV_LARGE | MACH_RCV_TIMEOUT,
                0,
                sizeof(req),
                self->excPort,
                50,
                MACH_PORT_NULL
            );

            if (kr == MACH_RCV_TIMED_OUT) {
                continue;
            }

            if (kr != KERN_SUCCESS) {
                break;
            }

            if (mach_exc_server(in, out)) {
                mach_msg(out, MACH_SEND_MSG, out->msgh_size, 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
            }
        }
        return nullptr;
    }
#endif
};

MachExceptionHandler::MachExceptionHandler() : m_impl(std::make_unique<Impl>()) {}

MachExceptionHandler::~MachExceptionHandler() {
    Stop();
}

MachExceptionHandler& MachExceptionHandler::Instance() {
    static MachExceptionHandler instance;
    return instance;
}

bool MachExceptionHandler::HandleBadAccess(std::uintptr_t faultAddress, bool isWrite) {
#ifdef __APPLE__
    if (m_impl && m_impl->callback) {
        return m_impl->callback(faultAddress, isWrite);
    }
#endif
    (void)faultAddress;
    (void)isWrite;
    return false;
}

bool MachExceptionHandler::Start(BadAccessCallback callback) {
#ifdef __APPLE__
    if (m_impl->running.load(std::memory_order_relaxed)) {
        return true;
    }

    m_impl->callback = std::move(callback);
    kern_return_t kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE, &m_impl->excPort);
    if (kr != KERN_SUCCESS) {
        return false;
    }

    kr = mach_port_insert_right(mach_task_self(), m_impl->excPort, m_impl->excPort, MACH_MSG_TYPE_MAKE_SEND);
    if (kr != KERN_SUCCESS) {
        mach_port_deallocate(mach_task_self(), m_impl->excPort);
        m_impl->excPort = MACH_PORT_NULL;
        return false;
    }

    kr = task_set_exception_ports(
        mach_task_self(),
        EXC_MASK_BAD_ACCESS,
        m_impl->excPort,
        EXCEPTION_DEFAULT | MACH_EXCEPTION_CODES,
        THREAD_STATE_NONE
    );
    if (kr != KERN_SUCCESS) {
        mach_port_deallocate(mach_task_self(), m_impl->excPort);
        m_impl->excPort = MACH_PORT_NULL;
        return false;
    }

    m_impl->running.store(true, std::memory_order_relaxed);
    if (pthread_create(&m_impl->listenerThread, nullptr, Impl::ServerLoop, m_impl.get()) != 0) {
        m_impl->running.store(false, std::memory_order_relaxed);
        mach_port_deallocate(mach_task_self(), m_impl->excPort);
        m_impl->excPort = MACH_PORT_NULL;
        return false;
    }

    return true;
#else
    (void)callback;
    return false;
#endif
}

void MachExceptionHandler::Stop() {
#ifdef __APPLE__
    if (!m_impl->running.load(std::memory_order_relaxed)) {
        return;
    }

    m_impl->running.store(false, std::memory_order_relaxed);
    pthread_join(m_impl->listenerThread, nullptr);

    if (m_impl->excPort != MACH_PORT_NULL) {
        mach_port_deallocate(mach_task_self(), m_impl->excPort);
        m_impl->excPort = MACH_PORT_NULL;
    }
#endif
}

bool MachExceptionHandler::IsRunning() const {
#ifdef __APPLE__
    return m_impl->running.load(std::memory_order_relaxed);
#else
    return false;
#endif
}

} // namespace Platform::MacOs
