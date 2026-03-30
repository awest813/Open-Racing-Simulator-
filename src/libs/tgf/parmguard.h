#ifndef TGF_PARMGUARD_H
#define TGF_PARMGUARD_H

#include <tgf.h>

/**
 * RAII guard for GfParmReadFile handles.
 * Automatically releases the handle when the guard goes out of scope.
 * Prevents leaks from early returns, exceptions, or overwritten pointers.
 *
 * Usage:
 *   GfParmHandleGuard guard(GfParmReadFile("foo.xml", GFPARM_RMODE_STD));
 *   // ... use guard.get() instead of raw void*
 *   // handle released automatically when guard destructs
 */
class GfParmHandleGuard {
public:
    explicit GfParmHandleGuard(void *handle) : _handle(handle) {}

    ~GfParmHandleGuard() {
        if (_handle) {
            GfParmReleaseHandle(_handle);
        }
    }

    GfParmHandleGuard(const GfParmHandleGuard &) = delete;
    GfParmHandleGuard &operator=(const GfParmHandleGuard &) = delete;

    GfParmHandleGuard(GfParmHandleGuard &&other) noexcept : _handle(other._handle) {
        other._handle = nullptr;
    }

    GfParmHandleGuard &operator=(GfParmHandleGuard &&other) noexcept {
        if (this != &other) {
            if (_handle) {
                GfParmReleaseHandle(_handle);
            }
            _handle = other._handle;
            other._handle = nullptr;
        }
        return *this;
    }

    void *get() const { return _handle; }

    void *release() {
        void *h = _handle;
        _handle = nullptr;
        return h;
    }

    operator void *() const { return _handle; }
    explicit operator bool() const { return _handle != nullptr; }

private:
    void *_handle;
};

#endif
