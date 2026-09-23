// Host test stub; not part of the Android build.
#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#define EX_ILLEGAL_ARGUMENT 1
#define EX_ILLEGAL_STATE 2
namespace ndk {
struct ScopedAStatus {
    int code;
    bool isOk() const { return code == 0; }
    static ScopedAStatus ok() { return {0}; }
    static ScopedAStatus fromExceptionCode(int n) { return {n}; }
};
struct SharedRefBase {
    template <class T, class... A> static std::shared_ptr<T> make(A&&... a) {
        return std::make_shared<T>(std::forward<A>(a)...);
    }
};
}  // namespace ndk
namespace aidl::android::hardware::power {
struct IPowerHintSession { virtual ~IPowerHintSession() = default; };
struct BnPowerHintSession : IPowerHintSession {
    virtual ndk::ScopedAStatus updateTargetWorkDuration(int64_t) = 0;
    virtual ndk::ScopedAStatus reportActualWorkDuration(const std::vector<WorkDuration>&) = 0;
    virtual ndk::ScopedAStatus pause() = 0;
    virtual ndk::ScopedAStatus resume() = 0;
    virtual ndk::ScopedAStatus close() = 0;
    virtual ndk::ScopedAStatus sendHint(SessionHint) = 0;
    virtual ndk::ScopedAStatus setThreads(const std::vector<int32_t>&) = 0;
};
}  // namespace aidl::android::hardware::power
