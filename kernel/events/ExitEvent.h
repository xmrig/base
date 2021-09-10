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

#ifndef XMRIG_EXITEVENT_H
#define XMRIG_EXITEVENT_H


#include "base/kernel/events/Event.h"


namespace xmrig {


class ExitEvent : public Event
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(ExitEvent)

    inline ExitEvent(int code) : m_code(code)   {}
    inline ~ExitEvent() override = default;

    inline int code() const                     { return m_code; }

protected:
    uint32_t type() const override              { return EXIT; }

#   ifdef APP_DEBUG
    void print() const override;
#   endif

private:
    const int m_code;
};


} // namespace xmrig


#endif // XMRIG_EXITEVENT_H
