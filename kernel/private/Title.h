/* XMRig
 * Copyright (c) 2018-2021 SChernykh   <https://github.com/SChernykh>
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

#ifndef XMRIG_TITLE_H
#define XMRIG_TITLE_H


#include "3rdparty/rapidjson/fwd.h"
#include "base/tools/String.h"


namespace xmrig {


class Arguments;
class IJsonReader;


class Title
{
public:
    Title() = default;
    Title(bool enabled) : m_enabled(enabled)            {}
    Title(const Arguments &arguments);
    Title(const IJsonReader &reader, const Title &current);

    inline bool isEnabled() const                       { return m_enabled; }
    inline bool isEqual(const Title &other) const       { return (m_enabled == other.m_enabled && m_value == other.m_value); }

    inline bool operator!=(const Title &other) const    { return !isEqual(other); }
    inline bool operator==(const Title &other) const    { return isEqual(other); }

    String value() const;
    void print() const;
    void save(rapidjson::Document &doc) const;

private:
    static constexpr const char *kKey = "title";

    bool m_enabled  = true;
    String m_value;
};


} // namespace xmrig


#endif /* XMRIG_TITLE_H */
