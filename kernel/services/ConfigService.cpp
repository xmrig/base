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

#include "base/kernel/services/ConfigService.h"
#include "base/io/json/JsonConfig.h"
#include "base/kernel/Events.h"
#include "base/kernel/events/ConfigEvent.h"
#include "base/kernel/interfaces/IConfigListener.h"
#include "base/kernel/Process.h"
#include "base/tools/Arguments.h"
#include "version.h"
#include "base/kernel/events/ConsoleEvent.h" // FIXME
#include "base/kernel/events/SaveEvent.h"


namespace xmrig {


class ConfigService::Private : public IConfigListener
{
public:
    inline bool open(const std::vector<String> &paths)
    {
        return std::any_of(paths.cbegin(), paths.cend(), [this](const String &fileName) { return main->open(fileName); });
    }

    std::shared_ptr<JsonConfig> main;

protected:
    void onConfig(IConfig * /*config*/, rapidjson::Document &doc) override
    {
        Process::events().post<ConfigEvent>(main, doc);
    }
};


} // namespace xmrig


xmrig::ConfigService::ConfigService() :
    d(std::make_shared<Private>())
{
    d->main = std::make_shared<JsonConfig>(0, "main", d.get());

    if (d->open(Process::arguments().values("-c")) || d->open(Process::arguments().values("--config"))) {
        return;
    }

    d->open({
        Process::locate(Process::DataLocation, "config.json"),
#       ifndef XMRIG_OS_WIN
        Process::locate(Process::HomeLocation,  "." APP_ID ".json"),
        Process::locate(Process::HomeLocation, ".config" XMRIG_DIR_SEPARATOR APP_ID ".json")
#       endif
    });
}


void xmrig::ConfigService::exec(int & /*rc*/)
{
    if (std::static_pointer_cast<IConfig>(d->main)->path().isNull()) {
        throw std::runtime_error("no valid configuration found");
    }
}


void xmrig::ConfigService::onEvent(uint32_t type, IEvent *event)
{
    if (type == IEvent::CONSOLE && static_cast<const ConsoleEvent *>(event)->command() == 's') { // FIXME
        Process::events().post<SaveEvent>(d->main);
    }

    if (type == IEvent::EXIT) {
        d->main->close();
    }

//    if (event->type() == IEvent::CONFIG) {
//        printf("ConfigService::onEvent<CONFIG>\n");
//    }
}
