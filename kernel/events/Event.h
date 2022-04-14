/* XMRig
 * Copyright (c) 2016-2022 SChernykh   <https://github.com/SChernykh>
 * Copyright (c) 2016-2022 XMRig       <https://github.com/xmrig>, <support@xmrig.com>
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

#ifndef XMRIG_EVENT_H
#define XMRIG_EVENT_H


#include "base/kernel/interfaces/IEvent.h"


#include <functional>


namespace xmrig {


class Event : public IEvent
{
public:
    XMRIG_DISABLE_COPY_MOVE(Event)

    inline Event()              = default;
    inline ~Event() override    = default;

    inline bool isRejected() const override     { return m_rejected; }
    inline int32_t route() const override       { return m_route; }
    inline uint64_t data() const override       { return 0; }
    inline void reject() override               { m_rejected = true; }

    const char *tag() const override;

protected:
#   ifdef APP_DEBUG
    void print() const override {}
#   endif

    bool m_rejected = false;
    int32_t m_route = -1;
};


} // namespace xmrig


#endif // XMRIG_EVENT_H
