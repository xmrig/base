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

#include "base/kernel/private/Title.h"
#include "3rdparty/rapidjson/document.h"
#include "base/io/Env.h"
#include "base/kernel/interfaces/IJsonReader.h"
#include "base/tools/Arguments.h"
#include "version.h"


#ifdef APP_DEBUG
#   include "base/io/log/Log.h"
#   include "base/kernel/Config.h"
#endif


xmrig::Title::Title(const Arguments &arguments)
{
#   ifdef XMRIG_DEPRECATED
    if (arguments.contains("--no-title")) {
        m_enabled = false;

        return;
    }
#   endif

    const size_t pos = arguments.pos("--title");
    if (pos) {
        m_value   = arguments.value(pos + 1U);
        m_enabled = !m_value.isEmpty();
    }
}


xmrig::Title::Title(const IJsonReader &reader, const Title &current)
{
    const auto &value = reader.getValue(kKey);
    if (value.IsBool()) {
        m_enabled = value.GetBool();
    }
    else if (value.IsString()) {
        m_value  = value.GetString();
        m_enabled = !m_value.isEmpty();
    }
    else {
        m_value   = current.m_value;
        m_enabled = current.m_enabled;
    }
}


xmrig::String xmrig::Title::value() const
{
    if (!isEnabled()) {
        return {};
    }

    if (m_value.isNull()) {
        return APP_NAME " " APP_VERSION;
    }

    return Env::expand(m_value);
}


void xmrig::Title::print() const
{
#   ifdef APP_DEBUG
    LOG_DEBUG("%s " MAGENTA_BOLD("TITLE")
              MAGENTA("<enabled=") CYAN("%d")
              MAGENTA(", value=") "\"%s\""
              MAGENTA(">"),
              Config::tag(), m_enabled, m_value.data());
#   endif
}


void xmrig::Title::save(rapidjson::Document &doc) const
{
    using namespace rapidjson;
    auto &allocator = doc.GetAllocator();

    doc.AddMember(StringRef(kKey), isEnabled() ? m_value.toJSON() : Value(m_enabled), allocator);
}
