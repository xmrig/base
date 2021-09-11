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

#ifndef XMRIG_PRIVATE_LOGCONFIG_H
#define XMRIG_PRIVATE_LOGCONFIG_H


#include "base/tools/String.h"


namespace xmrig {


class Arguments;
class IJsonReader;


class LogConfig
{
public:
    LogConfig() = default;
    LogConfig(const Arguments &arguments);
    LogConfig(const IJsonReader &reader, const LogConfig &current);

    inline bool isColors() const                            { return m_colors; }
    inline bool isSyslog() const                            { return m_syslog; }
    inline const String &file() const                       { return m_file; }
    inline uint32_t toggleVerbose()                         { m_verbose = (m_verbose + 1) % 6U; return m_verbose; }
    inline uint32_t verbose() const                         { return m_verbose; }

    inline bool operator!=(const LogConfig &other) const    { return !isEqual(other); }
    inline bool operator==(const LogConfig &other) const    { return isEqual(other); }

    bool isEqual(const LogConfig &other) const;
    void print() const;
    void save(rapidjson::Document &doc) const;

private:
    bool m_colors       = true;
    bool m_syslog       = false;
    String m_file;
    uint32_t m_verbose  = 0;
};


} // namespace xmrig


#endif // XMRIG_PRIVATE_LOGCONFIG_H
