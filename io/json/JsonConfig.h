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

#ifndef XMRIG_JSONCONFIG_H
#define XMRIG_JSONCONFIG_H


#include "base/kernel/Config.h"
#include "base/kernel/interfaces/IWatcherListener.h"


#include <memory>


namespace xmrig {


class IConfigListener;
class Watcher;


class JsonConfig : public Config, public IWatcherListener
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(JsonConfig)

    inline JsonConfig(uint32_t id, const String &name, IConfigListener *listener, bool watch = true) :
        Config(id, name, listener),
        m_watch(watch)
    {}

    inline ~JsonConfig() override = default;

    bool open(const String &fileName);
    void close();

protected:
    bool save(rapidjson::Document &doc) override;
    void onFileChanged(const String &fileName) override;

private:
    bool watch();

    bool m_watch    = true;
    std::shared_ptr<Watcher> m_watcher;
};


} // namespace xmrig


#endif // XMRIG_JSONCONFIG_H
