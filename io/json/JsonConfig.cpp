/* XMRig
 * Copyright (c) 2016-2022 SChernykh   <https://github.com/SChernykh>
 * Copyright (c) 2016-2022 XMRig       <https://github.com/xmrig>, <support@xmrig.com>
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

#include "base/io/json/JsonConfig.h"
#include "3rdparty/rapidjson/document.h"
#include "3rdparty/rapidjson/error/en.h"
#include "base/io/json/Json.h"
#include "base/io/log/Log.h"
#include "base/io/Watcher.h"
#include "base/kernel/interfaces/IConfigListener.h"


namespace xmrig {


static const constexpr char *kWatch     = "watch";


} // namespace xmrig


bool xmrig::JsonConfig::open(const String &fileName)
{
    if (fileName.isEmpty()) {
        return false;
    }

    using namespace rapidjson;
    Document doc;
    if (Json::get(fileName, doc) && setDoc(doc)) {
        m_watch = Json::getBool(doc, kWatch, m_watch);
        if (!m_watch && m_watcher) {
            m_watcher.reset();
        }

        if (m_path != fileName) {
            m_path = fileName;
        }

        m_listener->onConfig(this, doc);

        return watch();
    }

    if (doc.HasParseError()) {
        const size_t offset = doc.GetErrorOffset();
        size_t line         = 0;
        size_t pos          = 0;
        std::vector<std::string> s;

        if (Json::convertOffset(fileName, offset, line, pos, s)) {
            for (const auto &t : s) {
                LOG_ERR("%s %s", tag(), t.c_str());
            }

            std::string t;
            if (pos > 0) {
                t.assign(pos - 1, ' ');
            }

            t += '^';

            LOG_ERR("%s %s", tag(), t.c_str());
            LOG_ERR("%s " RED("%s<line:%zu, position:%zu>: ") RED_BOLD("\"%s\""), tag(), fileName.data(), line, pos, GetParseError_En(doc.GetParseError()));
        }
        else {
            LOG_ERR("%s " RED("%s<offset:%zu>: ") RED_BOLD("\"%s\""), tag(), fileName.data(), offset, GetParseError_En(doc.GetParseError()));
        }
    }
    else {
        LOG_WARN("%s " YELLOW("unable to open \"%s\""), tag(), fileName.data());
    }

    return false;
}


void xmrig::JsonConfig::close()
{
    m_watcher.reset();
}


void xmrig::JsonConfig::setPath(const String &path)
{
    m_path = path;
}


bool xmrig::JsonConfig::isValid() const
{
    return !m_path.isNull();
}


bool xmrig::JsonConfig::save(rapidjson::Document &doc)
{
    m_watcher.reset();

    using namespace rapidjson;
    doc.AddMember(StringRef(kWatch), m_watch, doc.GetAllocator());

    if (Json::save(m_path, doc)) {
        LOG_NOTICE("%s " WHITE_BOLD("\"%s\" ") GREEN_BOLD("(saved)"), tag(), m_path.data());

        return watch();
    }

    LOG_ERR("%s " WHITE_BOLD("\"%s\" ") RED_BOLD("(failed to save)"), tag(), m_path.data());

    return false;
}


void xmrig::JsonConfig::onFileChanged(const String &fileName)
{
    if (m_path != fileName) {
        return;
    }

    LOG_NOTICE("%s " WHITE_BOLD("\"%s\" ") YELLOW_BOLD("(changed on disk)"), tag(), m_path.data());

    open(m_path);
}


bool xmrig::JsonConfig::watch()
{
    if (m_watch && !m_watcher) {
        m_watcher = std::make_shared<Watcher>(m_path, this);
    }

    return true;
}
