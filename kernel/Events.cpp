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

#include <uv.h>


#include "base/kernel/Events.h"
#include "base/kernel/interfaces/IEvent.h"
#include "base/kernel/interfaces/IEventListener.h"


#include <algorithm>
#include <cassert>
#include <mutex>
#include <vector>


namespace xmrig {


class Events::Private
{
public:
    XMRIG_DISABLE_COPY_MOVE(Private)

    Private()
    {
        listeners.reserve(8);
        pending.reserve(16);

        idle.data = this;

        uv_idle_init(uv_default_loop(), &idle);
    }

    ~Private()
    {
        uv_idle_stop(&idle);
    }

    void flush()
    {
        mutex.lock();

        const auto queue = pending;
        pending.clear();
        uv_idle_stop(&idle);

        mutex.unlock();

        for (const auto &event : queue) {
#           ifdef APP_DEBUG
            event->print();
#           endif

            const uint32_t type = event->type();

            for (auto *listener : listeners) {
                listener->onEvent(type, event.get());
            }
        }
    }

    std::mutex mutex;
    std::vector<IEventListener *> listeners;
    std::vector<std::shared_ptr<IEvent> > pending;
    uv_idle_t idle;
};


} // namespace xmrig


xmrig::Events::Events() :
    d(std::make_shared<Private>())
{
}


xmrig::Events::~Events()
{
    assert(d->listeners.empty());
}


void xmrig::Events::send(IEventListener *listener, IEvent &event)
{
#   ifdef APP_DEBUG
    event.print();
#   endif

    listener->onEvent(event.type(), &event);
}


void xmrig::Events::addListener(IEventListener *listener)
{
    assert(listener);

    d->listeners.emplace_back(listener);
}


void xmrig::Events::post(std::shared_ptr<IEvent> &&event)
{
    std::lock_guard<std::mutex> lock(d->mutex);

    if (d->pending.empty()) {
        uv_idle_start(&d->idle, [](uv_idle_t *handle) { static_cast<Events::Private *>(handle->data)->flush(); });
    }

    d->pending.emplace_back(std::move(event));
}


void xmrig::Events::removeListener(IEventListener *listener)
{
    auto it = std::find(d->listeners.begin(), d->listeners.end(), listener);
    if (it != d->listeners.end()) {
        d->listeners.erase(it);
    }
}


void xmrig::Events::send(IEvent &event)
{
#   ifdef APP_DEBUG
    event.print();
#   endif

    const uint32_t type = event.type();

    for (auto *listener : d->listeners) {
        listener->onEvent(type, &event);
    }
}
