/* XMRig
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

#include "base/kernel/private/LogConfig.h"
#include "3rdparty/rapidjson/document.h"
#include "base/kernel/interfaces/IJsonReader.h"
#include "base/tools/Arguments.h"


#ifdef APP_DEBUG
#   include "base/io/log/Log.h"
#   include "base/kernel/Config.h"
#endif


#include <algorithm>


namespace xmrig {


static const constexpr char *kColors    = "colors";
static const constexpr char *kLogFile   = "log_file";
static const constexpr char *kVerbose   = "verbose";

#ifdef HAVE_SYSLOG_H
static const constexpr char *kSyslog    = "syslog";
#endif


} // namespace xmrig


xmrig::LogConfig::LogConfig(const Arguments &arguments)
{
    const size_t verbose = arguments.pos("--verbose");
    if (verbose) {
        const auto &value = arguments.value(verbose + 1U);
        m_verbose = !value.isEmpty() ? std::min(strtoul(value, nullptr, 0), 5UL) : 1U;
    }

    m_colors = !arguments.contains("--no-color");
    m_file   = arguments.value("-l", "--log-file");

#   ifdef HAVE_SYSLOG_H
    m_syslog = arguments.contains("-S", "--syslog");
#   endif
}


xmrig::LogConfig::LogConfig(const IJsonReader &reader, const LogConfig &current)
{
    m_colors  = reader.getBool(kColors, current.m_colors);
    m_file    = reader.getString(kLogFile, current.m_file);
    m_verbose = std::min(reader.getUint(kVerbose, current.m_verbose), 5U);

#   ifdef HAVE_SYSLOG_H
    m_syslog  = reader.getBool(kSyslog, current.m_syslog);
#   endif

#   ifdef XMRIG_DEPRECATED
    if (m_file.isNull()) {
        m_file = reader.getString("log-file", m_file);
    }
#   endif
}


bool xmrig::LogConfig::isEqual(const LogConfig &other) const
{
    return (m_colors        == other.m_colors
            && m_syslog     == other.m_syslog
            && m_file       == other.m_file
            && m_verbose    == other.m_verbose
            );
}


void xmrig::LogConfig::print() const
{
#   ifdef APP_DEBUG
    LOG_DEBUG("%s " MAGENTA_BOLD("LOG")
              MAGENTA("<colors=") CYAN("%d")
              MAGENTA(", syslog=") CYAN("%d")
              MAGENTA(", file=") "\"%s\""
              MAGENTA(", verbose=") CYAN("%u")
              MAGENTA(">"),
              Config::tag(), isColors(), isSyslog(), file().data(), verbose());
#   endif
}


void xmrig::LogConfig::save(rapidjson::Document &doc) const
{
    using namespace rapidjson;
    auto &allocator = doc.GetAllocator();

    doc.AddMember(StringRef(kColors),   m_colors ? Value(kNullType) : Value(m_colors), allocator);
    doc.AddMember(StringRef(kVerbose),  m_verbose ? Value(m_verbose) : Value(kNullType), allocator);
    doc.AddMember(StringRef(kLogFile),  m_file.toJSON(), allocator);

#   ifdef HAVE_SYSLOG_H
    doc.AddMember(StringRef(kSyslog),   m_syslog ? Value(m_syslog) : Value(kNullType), allocator);
#   endif
}
