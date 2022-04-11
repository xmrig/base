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

#include "base/kernel/services/ConsoleService.h"
#include "base/io/Console.h"
#include "base/io/Signals.h"
#include "base/kernel/events/ConfigEvent.h"
#include "base/kernel/events/SaveEvent.h"
#include "base/kernel/interfaces/IEvent.h"
#include "base/kernel/private/Title.h"
#include "base/kernel/Process.h"


namespace xmrig {


class ConsoleService::Private
{
public:
    std::shared_ptr<Console> console;

#   ifdef XMRIG_OS_WIN
    Title title;

    void apply(const Title &next)
    {
        if (title == next) {
            return;
        }

        title = next;
        title.print();

        console->setTitle(title);
    }
#   endif
};


} // namespace xmrig


xmrig::ConsoleService::ConsoleService() :
    d(std::make_shared<Private>())
{
    d->console = std::make_shared<Console>();

#   ifdef XMRIG_OS_WIN
    d->apply(Process::arguments());

    if (d->title.isEnabled()) {
        d->console->setTitle(d->title);
    }
#   endif
}


void xmrig::ConsoleService::onEvent(uint32_t type, IEvent *event)
{
    if (type == IEvent::EXIT) {
        return d->console.reset();
    }

#   ifdef XMRIG_OS_WIN
    if (type == IEvent::CONFIG && event->data() == 0 && !event->isRejected()) {
        return d->apply({ *static_cast<const ConfigEvent *>(event)->reader(), d->title });
    }

    SaveEvent::handle(type, event, 0, [this](rapidjson::Document &doc) { d->title.save(doc); });
#   endif
}
