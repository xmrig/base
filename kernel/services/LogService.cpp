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

#include "base/kernel/services/LogService.h"
#include "base/io/log/backends/ConsoleLog.h"
#include "base/io/log/backends/FileLog.h"
#include "base/io/log/Log.h"
#include "base/kernel/events/ConfigEvent.h"
#include "base/kernel/events/LogEvent.h"
#include "base/kernel/events/SaveEvent.h"
#include "base/kernel/private/LogConfig.h"
#include "base/kernel/Process.h"
#include "base/kernel/Config.h"


#ifdef HAVE_SYSLOG_H
#   include "base/io/log/backends/SysLog.h"
#endif


namespace xmrig {


class LogService::Private
{
public:
    void print(const LogEvent *event) const
    {
        print(console, event);
        print(file, event);
        print(syslog, event);
    }

    void apply(const LogConfig &next)
    {
        if (config == next) {
            return;
        }

        Log::setColors(next.isColors());
        Log::setVerbose(next.verbose());

        if (config.file() != next.file()) {
            file = next.file().isEmpty() ? nullptr : std::make_shared<FileLog>(next.file());
        }

#       ifdef HAVE_SYSLOG_H
        if (config.isSyslog() != next.isSyslog()) {
            syslog = next.isSyslog() ? std::make_shared<SysLog>() : nullptr;
        }
#       endif

        config = next;
        config.print();
    }

    void toggleVerbose()
    {
        Log::setVerbose(config.toggleVerbose());

        LOG_NOTICE("%s " WHITE_BOLD("verbose: ") CYAN_BOLD("%u"), Config::tag(), Log::verbose());
    }

    LogConfig config;
    std::shared_ptr<ILogBackend> console;
    std::shared_ptr<ILogBackend> file;
    std::shared_ptr<ILogBackend> syslog;

private:
    static void print(const std::shared_ptr<ILogBackend> &backend, const LogEvent *event)
    {
        if (backend) {
            backend->print(event->timestamp(), event->level(), event->line(true), event->offset(true), event->size(true), true);
            backend->print(event->timestamp(), event->level(), event->line(false), event->offset(false), event->size(false), false);
        }
    }
};


} // namespace xmrig


xmrig::LogService::LogService() :
    d(std::make_shared<Private>())
{
    if (!Log::isBackground()) {
        d->console = std::make_shared<ConsoleLog>();
    }

    d->apply(Process::arguments());
}


void xmrig::LogService::onEvent(uint32_t type, IEvent *event)
{
    if (type == IEvent::LOG) {
        return d->print(static_cast<const LogEvent *>(event));
    }

    if (type == IEvent::CONFIG && event->data() == 0) {
        d->apply({ *static_cast<const ConfigEvent *>(event)->reader(), d->config });
    }

    if (type == IEvent::SAVE && event->data() == 0) {
        d->config.save(static_cast<SaveEvent *>(event)->doc());
    }

    if (type == IEvent::CONSOLE && (event->data() == 'v' || event->data() == 'V')) {
        d->toggleVerbose();
    }
}
