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

#include "base/kernel/services/SignalService.h"
#include "base/io/Signals.h"
#include "base/kernel/interfaces/IEvent.h"


namespace xmrig {


class SignalService::Private
{
public:
    std::shared_ptr<Signals> signals;
};


} // namespace xmrig


xmrig::SignalService::SignalService() :
    d(std::make_shared<Private>())
{
    d->signals = std::make_shared<Signals>();
}


void xmrig::SignalService::onEvent(uint32_t type, IEvent * /*event*/)
{
    if (type == IEvent::EXIT) {
        return d->signals.reset();
    }
}
