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

#include "base/kernel/events/ConfigEvent.h"
#include "base/io/log/Log.h"


bool xmrig::ConfigEvent::handle(uint32_t type, IEvent *event, uint32_t id, const std::function<void(const IJsonReader &reader, bool valid)> &callback)
{
    if (type == CONFIG && event->data() == id) {
        callback(*static_cast<ConfigEvent *>(event)->reader(), !event->isRejected());

        return true;
    }

    return false;
}


#ifdef APP_DEBUG
void xmrig::ConfigEvent::print() const
{
    LOG_DEBUG("%s " MAGENTA_BOLD("CONFIG")
              MAGENTA("<id=") CYAN("%u")
              MAGENTA(", name=") "\"%s\""
              MAGENTA(", path=") "\"%s\""
              MAGENTA(">")
              CYAN(" {%u}"),
              tag(), id(), name().data(), path().data(), value().MemberCount());
}
#endif
