// Host test stub; not part of the Android build.
#pragma once
struct LogSink { template <class T> LogSink& operator<<(const T&) { return *this; } };
#define LOG(level) LogSink{}
