/* XMRig
 * Copyright (c) 2019      Spudz76     <https://github.com/Spudz76>
 * Copyright (c) 2018-2021 SChernykh   <https://github.com/SChernykh>
 * Copyright (c) 2016-2021 XMRig       <https://github.com/xmrig>, <support@xmrig.com>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef XMRIG_OS_WIN
#   include <winsock2.h>
#   include <windows.h>
#endif


#include <algorithm>
#include <cassert>
#include <cstdarg>
#include <cstring>
#include <ctime>
#include <mutex>


#include "base/io/log/Log.h"
#include "base/kernel/Events.h"
#include "base/kernel/events/LogEvent.h"
#include "base/kernel/private/LogConfig.h"
#include "base/kernel/Process.h"
#include "base/tools/Chrono.h"


namespace xmrig {


bool Log::m_background      = false;
bool Log::m_colors          = true;
uint32_t Log::m_verbose     = 0;


static char buf[Log::kMaxBufferSize]{};
static std::mutex mutex;


static const char *colors_map[] = {
    RED_BOLD_S      "E ", // EMERG
    RED_BOLD_S      "A ", // ALERT
    RED_BOLD_S      "C ", // CRIT
    RED_S           "E ", // ERR
    YELLOW_S        "W ", // WARNING
    WHITE_BOLD_S    "N ", // NOTICE
                    "I ", // INFO
                    "1 ", // V1
                    "2 ", // V2
                    "3 ", // V3
                    "4 ", // V4
#   ifdef XMRIG_OS_WIN
    BLACK_BOLD_S    "5 ", // V5
    BLACK_BOLD_S    "D "  // DEBUG
#   else
    BRIGHT_BLACK_S  "5 ", // V5
    BRIGHT_BLACK_S  "D "  // DEBUG
#   endif
};


static void log_endl(size_t &size)
{
#   ifdef XMRIG_OS_WIN
    memcpy(buf + size, CLEAR "\r\n", 7);
    size += 6;
#   else
    memcpy(buf + size, CLEAR "\n", 6);
    size += 5;
#   endif
}


static void log_color(Log::Level level, size_t &size)
{
    if (level == Log::NONE) {
        return;
    }

    const char *color = colors_map[level];
    if (color == nullptr) {
        return;
    }

    const size_t s = strlen(color);
    memcpy(buf + size, color, s); // NOLINT(bugprone-not-null-terminated-result)

    size += s;
}


static uint64_t log_timestamp(Log::Level level, size_t &size, size_t &offset)
{
    const uint64_t ms = Chrono::currentMSecsSinceEpoch();

    if (level == Log::NONE) {
        return ms;
    }

    time_t now = ms / 1000;
    tm stime{};

#   ifdef XMRIG_OS_WIN
    localtime_s(&stime, &now);
#   else
    localtime_r(&now, &stime);
#   endif

    const int rc = snprintf(buf, sizeof(buf) - 1, "[%d-%02d-%02d %02d:%02d:%02d" BLACK_BOLD(".%03d") "] ",
                            stime.tm_year + 1900,
                            stime.tm_mon + 1,
                            stime.tm_mday,
                            stime.tm_hour,
                            stime.tm_min,
                            stime.tm_sec,
                            static_cast<int>(ms % 1000)
                            );

    if (rc > 0) {
        size = offset = static_cast<size_t>(rc);
    }

    return ms;
}



static void log_print(Log::Level level, const char *fmt, va_list args)
{
    size_t size   = 0;
    size_t offset = 0;

    std::lock_guard<std::mutex> lock(mutex);

    const uint64_t ts = log_timestamp(level, size, offset);
    log_color(level, size);

    const int rc = vsnprintf(buf + size, sizeof(buf) - offset - 32, fmt, args);
    if (rc < 0) {
        return;
    }

    size += std::min(static_cast<size_t>(rc), sizeof(buf) - offset - 32);
    log_endl(size);

    Process::events().post<LogEvent>(ts, level, buf, offset, size);
}


} // namespace xmrig


void xmrig::Log::print(const char *fmt, ...)
{
    va_list args{};
    va_start(args, fmt);

    log_print(NONE, fmt, args);

    va_end(args);
}


void xmrig::Log::print(Level level, const char *fmt, ...)
{
    va_list args{};
    va_start(args, fmt);

    log_print(level, fmt, args);

    va_end(args);
}


void xmrig::Log::setVerbose(uint32_t verbose)
{
    m_verbose = std::min(verbose, static_cast<uint32_t>(LogConfig::kMaxVerbose));
}
