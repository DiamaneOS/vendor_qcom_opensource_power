// Host regression test for hint-session boost lifecycle.
// Compiles the real PowerHintSession.cpp against stubs and a fake performance
// backend; it checks control flow, not Binder or device power behavior.
#include "PowerHintSession.h"

#include <cstdio>
#include <cstdlib>
#include <map>
#include <set>

using aidl::android::hardware::power::SessionHint;

static std::map<int, int> boosts;  // handle -> requested boost level
static std::set<int> pipelines;
static int nextHandle = 1;

extern "C" int perf_hint_enable(int, int level) { boosts[nextHandle] = level; return nextHandle++; }
extern "C" int interaction_with_handle(int, int, int, int*) { pipelines.insert(nextHandle); return nextHandle++; }
extern "C" void release_request(int h) { boosts.erase(h); pipelines.erase(h); }

static int failures = 0;
#define EXPECT(cond, what) do { if (!(cond)) { std::printf("FAIL %s: %s\n", __func__, what); ++failures; } } while (0)

static void reset() { boosts.clear(); pipelines.clear(); }
static int level() { return boosts.empty() ? 0 : boosts.begin()->second; }

static void activeCloseReleasesEverything() {
    reset();
    {
        PowerHintSessionImpl s(100, 100, {101});
        s.sendHint(SessionHint::CPU_LOAD_UP);
        EXPECT(boosts.size() == 1, "boost acquired");
        s.close();
        EXPECT(boosts.empty() && pipelines.empty(), "released on close");
    }
    EXPECT(boosts.empty() && pipelines.empty(), "nothing held after destruction");
}

static void pauseReleasesAndPausedCloseLeaksNothing() {
    reset();
    {
        PowerHintSessionImpl s(100, 100, {101});
        s.sendHint(SessionHint::CPU_LOAD_UP);
        s.pause();
        EXPECT(boosts.empty() && pipelines.empty(), "released on pause");
        s.close();
        EXPECT(boosts.empty() && pipelines.empty(), "nothing held after paused close");
    }
    EXPECT(boosts.empty() && pipelines.empty(), "nothing held after destruction");
}

static void resumeRestoresPrePauseLevel() {
    reset();
    PowerHintSessionImpl s(100, 100, {101});
    s.sendHint(SessionHint::CPU_LOAD_UP);
    s.sendHint(SessionHint::CPU_LOAD_UP);
    int before = level();
    s.pause();
    s.resume();
    EXPECT(boosts.size() == 1 && level() == before, "pre-pause level restored");
    EXPECT(pipelines.size() == 1, "pipeline restored");
    s.close();
    EXPECT(boosts.empty() && pipelines.empty(), "released on close");
}

static void hintsWhilePausedAreIgnored() {
    reset();
    PowerHintSessionImpl s(100, 100, {101});
    s.pause();
    EXPECT(s.sendHint(SessionHint::CPU_LOAD_UP).isOk(), "paused hint returns ok");
    EXPECT(boosts.empty(), "paused hint acquires nothing");
    s.close();
}

static void destructionWithoutCloseReleases() {
    reset();
    {
        PowerHintSessionImpl s(100, 100, {101});
        s.sendHint(SessionHint::CPU_LOAD_UP);
    }
    EXPECT(boosts.empty() && pipelines.empty(), "released by destructor");
}

static void repeatedPauseResumeDoesNotLeak() {
    reset();
    PowerHintSessionImpl s(100, 100, {101});
    s.sendHint(SessionHint::CPU_LOAD_UP);
    for (int i = 0; i < 5; ++i) {
        s.pause();
        EXPECT(boosts.empty() && pipelines.empty(), "released on each pause");
        s.resume();
        EXPECT(boosts.size() == 1 && pipelines.size() == 1, "exactly one of each after resume");
    }
    s.close();
    EXPECT(boosts.empty() && pipelines.empty(), "released on close");
}

int main() {
    activeCloseReleasesEverything();
    pauseReleasesAndPausedCloseLeaksNothing();
    resumeRestoresPrePauseLevel();
    hintsWhilePausedAreIgnored();
    destructionWithoutCloseReleases();
    repeatedPauseResumeDoesNotLeak();
    std::printf(failures ? "%d FAILED\n" : "PASS\n", failures);
    return failures ? EXIT_FAILURE : EXIT_SUCCESS;
}
