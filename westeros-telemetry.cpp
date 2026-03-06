/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2026 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "westeros-telemetry.h"

namespace WstUtils
{
    namespace {
        bool inited = false;
    }

    bool telemetryEnsureAvailable()
    {
#ifdef ENABLE_TELEMETRY_LOGGING
        if (!inited && t2_init && t2_event_s)
        {
            t2_init((char*)"WesterosCompositor");
            inited = true;
        }

        return inited;
#else
        return false;
#endif
    }

    void telemetryInit()
    {
        telemetryEnsureAvailable();
    }

    void telemetrySendMessage(const char* marker, const char* value)
    {
#ifdef ENABLE_TELEMETRY_LOGGING
        if (telemetryEnsureAvailable() && marker && value)
        {
            t2_event_s((char*)marker, (char*)value);
        }
#else
        (void)marker;
        (void)value;
#endif
    }
}
