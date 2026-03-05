#pragma once

#ifdef ENABLE_TELEMETRY_LOGGING
extern "C" {
__attribute__((weak)) void t2_init(char* component);
__attribute__((weak)) int t2_event_s(char* marker, char* value);
}
#endif

namespace WstUtils
{
    struct Telemetry
    {
        static bool ensureAvailable()
        {
#ifdef ENABLE_TELEMETRY_LOGGING
            static bool initialized = false;
            static bool available = false;

            if (!initialized)
            {
                if (t2_init && t2_event_s)
                {
                    t2_init((char*)"WesterosCompositor");
                    available = true;
                }
                initialized = true;
            }

            return available;
#else
            return false;
#endif
        }

        static void init()
        {
            ensureAvailable();
        }

        static void sendMessage(const char* marker, const char* value)
        {
#ifdef ENABLE_TELEMETRY_LOGGING
            if (ensureAvailable() && marker && value)
            {
                t2_event_s((char*)marker, (char*)value);
            }
#else
            (void)marker;
            (void)value;
#endif
        }
    };
}
