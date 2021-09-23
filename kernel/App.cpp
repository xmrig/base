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


#include "base/kernel/App.h"
#include "base/io/log/Log.h"
#include "base/kernel/Events.h"
#include "base/kernel/Process.h"
#include "base/kernel/Service.h"
#include "base/kernel/services/ConfigService.h"
#include "base/kernel/services/ConsoleService.h"
#include "base/kernel/services/LogService.h"
#include "base/kernel/services/SignalService.h"
#include "base/net/tools/NetBuffer.h"


#include <algorithm>
#include <vector>



namespace xmrig {


class App::Private
{
public:
    XMRIG_DISABLE_COPY_MOVE(Private)

    Private()
    {
        services.reserve(8);
    }

    ~Private()
    {
        for (auto *service : services) {
            Process::events().removeListener(service);

            delete service;
        }
    }

    std::vector<Service *> services;
};


} // namespace xmrig



xmrig::App::App() :
    d(std::make_shared<Private>())
{
    add<LogService>();
    add<SignalService>();

    if (!Log::isBackground()) {
        add<ConsoleService>();
    }

    add<ConfigService>();
}


xmrig::App::~App()
{
    NetBuffer::destroy();
}


const char *xmrig::App::tag()
{
    static const char *tag = MAGENTA_BG_BOLD(WHITE_BOLD_S " app     ");

    return tag;
}


int xmrig::App::exec()
{
    int rc = 0;

    try {
        for (auto *service : d->services) {
            service->exec(rc);
        }
    } catch (std::exception &ex) {
        LOG_CRIT("%s " RED_BOLD("exec failed: \"%s\""), tag(), ex.what());
        rc = std::max(rc, 1);

        Process::exit(rc);
    }

    rc = uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    uv_loop_close(uv_default_loop());

    return Process::exitCode() ? Process::exitCode() : rc;
}


void xmrig::App::add(Service *service)
{
    d->services.emplace_back(service);

    Process::events().addListener(service);
}


void xmrig::App::remove(Service *service)
{
    auto it = std::find(d->services.begin(), d->services.end(), service);
    if (it != d->services.end()) {
        d->services.erase(it);
    }

    Process::events().removeListener(service);
}
