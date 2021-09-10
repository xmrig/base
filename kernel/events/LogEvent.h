/* XMRig
 * Copyright (c) 2016-2021 SChernykh   <https://github.com/SChernykh>
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

#ifndef XMRIG_LOGEVENT_H
#define XMRIG_LOGEVENT_H


#include "base/kernel/events/Event.h"


#include <string>


namespace xmrig {


class LogEvent : public Event
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(LogEvent)

    inline LogEvent(uint64_t timestamp, int level, const char *line, size_t offset, size_t size) :
        m_level(level),
        m_offset(offset),
        m_data(line, size),
        m_timestamp(timestamp),
        m_text(line, size)
    {
        size_t i = 0;
        while ((i = m_text.find("\x1B[")) != std::string::npos) {
            m_text.erase(i, m_text.find('m', i) - i + 1);
        }
    }

    inline ~LogEvent() override = default;

    inline const char *line(bool colors) const  { return colors ? m_data.c_str() : m_text.c_str(); }
    inline int level() const                    { return m_level; }
    inline size_t offset(bool colors) const     { return m_offset ? (colors ? m_offset : (m_offset - 11)) : 0; }
    inline size_t size(bool colors) const       { return colors ? m_data.size() : m_text.size(); }
    inline uint64_t timestamp() const           { return m_timestamp; }

protected:
    uint32_t type() const override              { return LOG; }

private:
    const int m_level;
    const size_t m_offset;
    const std::string m_data;
    const uint64_t m_timestamp;
    std::string m_text;
};


} // namespace xmrig


#endif // XMRIG_LOGEVENT_H
