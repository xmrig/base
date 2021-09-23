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

#ifndef XMRIG_CONFIGEVENT_H
#define XMRIG_CONFIGEVENT_H


#include "3rdparty/rapidjson/document.h"
#include "base/io/json/Json.h"
#include "base/kernel/events/Event.h"
#include "base/kernel/interfaces/IConfig.h"


namespace xmrig {


class ConfigEvent : public Event
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(ConfigEvent)

    inline ConfigEvent(const std::shared_ptr<IConfig> &config, rapidjson::Document &doc) :
        m_doc(std::move(doc)),
        m_config(config),
        m_id(config->id())
    {
        m_reader = std::make_shared<JsonReader>(m_doc);
    }

    inline ConfigEvent(const std::shared_ptr<IConfig> &config) :
        m_doc(rapidjson::kObjectType),
        m_config(config),
        m_id(config->id())
    {}

    inline ~ConfigEvent() override = default;

    inline bool isRejected() const override                 { return !m_config->isValid(); }
    inline const IJsonReader *reader() const                { assert(m_reader.get()); return m_reader.get(); }
    inline const rapidjson::Value &value() const            { return m_doc; }
    inline const std::shared_ptr<IConfig> &config() const   { return m_config; }
    inline const String &name() const                       { return m_config->name(); }
    inline const String &path() const                       { return m_config->path(); }
    inline uint32_t id() const                              { return m_id; }

protected:
    inline uint32_t type() const override                   { return CONFIG; }
    inline uint64_t data() const override                   { return m_id; }

#   ifdef APP_DEBUG
    void print() const override;
#   endif

private:
    const rapidjson::Document m_doc;
    const std::shared_ptr<IConfig> m_config;
    const uint32_t m_id;
    std::shared_ptr<JsonReader> m_reader;
};


} // namespace xmrig


#endif // XMRIG_CONFIGEVENT_H
