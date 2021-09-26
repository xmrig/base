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

#ifndef XMRIG_PRINTEVENT_H
#define XMRIG_PRINTEVENT_H


#include "base/kernel/events/Event.h"


namespace xmrig {


class PrintEvent : public Event
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(PrintEvent)

    inline PrintEvent(uint64_t now, uint64_t ticks) : m_now(now), m_ticks(ticks)    {}
    inline ~PrintEvent() override = default;

    inline uint64_t now() const     { return m_now; }
    inline uint64_t ticks() const   { return m_ticks; }

protected:
    uint32_t type() const override  { return PRINT; }

#   ifdef APP_DEBUG
    void print() const override;
#   endif

private:
    const uint64_t m_now;
    const uint64_t m_ticks;
};


} // namespace xmrig


#endif // XMRIG_PRINTEVENT_H
