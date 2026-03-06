#pragma once

#ifdef ENABLE_TELEMETRY_LOGGING
extern "C" {
__attribute__((weak)) void t2_init(char* component);
__attribute__((weak)) int t2_event_s(char* marker, char* value);
}
#endif

namespace WstUtils
{
    bool telemetryEnsureAvailable();
    void telemetryInit();
    void telemetrySendMessage(const char* marker, const char* value);
}
