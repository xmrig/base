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

#ifndef XMRIG_CONFIG_H
#define XMRIG_CONFIG_H


#include "base/kernel/interfaces/IConfig.h"
#include "base/tools/String.h"


namespace xmrig {


class IConfigListener;


class Config : public IConfig
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(Config)

    inline Config(uint32_t id, const String &name, IConfigListener *listener) :
        m_listener(listener),
        m_name(name),
        m_id(id)
    {}

    inline ~Config() override = default;

    static const char *tag();

protected:
    inline const String &name() const override  { return m_name; }
    inline const String &path() const override  { return m_path; }
    inline uint32_t id() const override         { return m_id; }

    bool setDoc(rapidjson::Document &doc);

    IConfigListener *m_listener;
    String m_path;

private:
    const String m_name;
    const uint32_t m_id;
};


} // namespace xmrig


#endif // XMRIG_CONFIG_H
