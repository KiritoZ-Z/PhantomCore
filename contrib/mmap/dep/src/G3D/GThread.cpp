/**
 @file GThread.cpp

 GThread class.

 @created 2005-09-24
 @edited  2005-10-22
 */

#include "G3D/GThread.h"
#include "G3D/debugAssert.h"


namespace G3D {

namespace _internal {

using G3D::GThread;

class GThreadPrivate {
public:
    bool running;
    bool completed;

#   ifdef G3D_WIN32
    HANDLE event;
#   endif

    GThreadPrivate():
        running(false),
        completed(false) {

#   ifdef G3D_WIN32
        event = NULL;
#   endif

    }

#   ifdef G3D_WIN32
    
    static DWORD WINAPI GThreadProc(LPVOID param) {
        GThread* current = (GThread*)param;
        debugAssert(current->pthread->event);
        current->pthread->running = true;
        current->pthread->completed = false;
        current->threadMain();
        current->pthread->running = false;
        current->pthread->completed = true;
        ::SetEvent(current->pthread->event);
        return 0;
    }
    
#   else
    
    static void* GThreadProc(void* param) {
        GThread* current = (GThread*)param;
        current->pthread->running = true;
        current->pthread->completed = false;
        current->threadMain();
        current->pthread->running = false;
        current->pthread->completed = true;
        return (void*)NULL;
    }

#   endif
};

class BasicThread: public GThread {
private:
    void (*wrapperProc)();
public:
    BasicThread(const std::string& name, void (*proc)()):
        GThread(name), wrapperProc(proc) { }
protected:
    virtual void threadMain() {
        wrapperProc();
    }
};

} // namespace _internal


GThread::GThread(const std::string& name):
    handle(NULL),
    _name(name) {

    pthread = new _internal::GThreadPrivate;
}

GThread::~GThread() {
#   ifdef G3D_WIN32
#       pragma warning( push )
#       pragma warning( disable : 4127 )
#   endif
    alwaysAssertM(!pthread->running, "Deleting thread while running.");
#   ifdef G3D_WIN32
#       pragma warning( pop )

    if (pthread->event) {
        ::CloseHandle(pthread->event);
    }

#   endif
    delete pthread;
}

GThreadRef GThread::create(const std::string& name, void (*proc)()) {
    return new _internal::BasicThread(name, proc);
}

bool GThread::start() {

    debugAssertM(!pthread->completed, "Thread has already executed.");

    if (pthread->completed) {
        return false;
    }

#   ifdef G3D_WIN32
    DWORD threadId;

    pthread->event = ::CreateEvent(NULL, TRUE, FALSE, NULL);
    debugAssert(pthread->event);

    handle = ::CreateThread(
        NULL,
        0,
        &_internal::GThreadPrivate::GThreadProc,
        this,
        0,
        &threadId);

    if (handle == NULL) {
        ::CloseHandle(pthread->event);
        pthread->event = NULL;
    }

    return (handle != NULL);
#   else
    if (!pthread_create((pthread_t*)&handle,
                        NULL,
                        &_internal::GThreadPrivate::GThreadProc, 
                        this)) {
        return true;
    } else {
        handle = NULL;
        return false;
    }
#   endif
}

void GThread::terminate() {
    if (handle) {
#       ifdef G3D_WIN32
        ::TerminateThread(handle, 0);
#       else
        pthread_kill((pthread_t)handle, SIGSTOP);
#       endif
        handle = NULL;
    }
}

bool GThread::running() {
    return pthread->running;
}

bool GThread::completed() {
    return pthread->completed;
}

void GThread::waitForCompletion() {
#   ifdef G3D_WIN32
    debugAssert(pthread->event);
    ::WaitForSingleObject(pthread->event, INFINITE);
#   else
    debugAssert(handle);
    pthread_join((pthread_t)handle, NULL);
#   endif
}


GMutex::GMutex() {
#   ifdef G3D_WIN32
    ::InitializeCriticalSection(&handle);
#   else
    pthread_mutex_init(&handle, NULL);
#   endif
}

GMutex::~GMutex() {
    //TODO: Debug check for locked
#   ifdef G3D_WIN32
    ::DeleteCriticalSection(&handle);
#   else
    pthread_mutex_destroy(&handle);
#   endif
}

//bool GMutex::tryLock() {
//#   ifdef G3D_WIN32
//    return ::TryEnterCriticalSection(&handle);
//#   else
//    return pthread_mutex_trylock(&handle);
//#   endif
//}

void GMutex::lock() {
#   ifdef G3D_WIN32
    ::EnterCriticalSection(&handle);
#   else
    pthread_mutex_lock(&handle);
#   endif
}

void GMutex::unlock() {
#   ifdef G3D_WIN32
    ::LeaveCriticalSection(&handle);
#   else
    pthread_mutex_unlock(&handle);
#   endif
}

} // namespace G3D
