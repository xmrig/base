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


#include "base/io/Watcher.h"
#include "base/kernel/interfaces/IWatcherListener.h"
#include "base/tools/Handle.h"
#include "base/tools/Timer.h"


xmrig::Watcher::Watcher(const String &path, IWatcherListener *listener) :
    m_listener(listener),
    m_path(path)
{
    m_timer = new Timer(this);

    m_fsEvent = new uv_fs_event_t;
    m_fsEvent->data = this;
    uv_fs_event_init(uv_default_loop(), m_fsEvent);

    start();
}


xmrig::Watcher::~Watcher()
{
    delete m_timer;

    Handle::close(m_fsEvent);
}


void xmrig::Watcher::onFsEvent(uv_fs_event_t *handle, const char *filename, int, int)
{
    if (!filename) {
        return;
    }

    static_cast<Watcher *>(handle->data)->queueUpdate();
}


void xmrig::Watcher::queueUpdate()
{
    m_timer->stop();
    m_timer->start(kDelay, 0);
}


void xmrig::Watcher::reload()
{
    m_listener->onFileChanged(m_path);

#   ifndef _WIN32
    uv_fs_event_stop(m_fsEvent);
    start();
#   endif
}


void xmrig::Watcher::start()
{
    uv_fs_event_start(m_fsEvent, xmrig::Watcher::onFsEvent, m_path, 0);
}
