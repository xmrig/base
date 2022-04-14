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

#include "base/kernel/services/TlsService.h"
#include "base/io/log/Log.h"
#include "base/kernel/events/ConfigEvent.h"
#include "base/kernel/events/SaveEvent.h"
#include "base/kernel/Process.h"
#include "base/net/tls/TlsConfig.h"
#include "base/net/tls/TlsContext.h"


namespace xmrig {


static const char *tag = GREEN_BG_BOLD(WHITE_BOLD_S " tls     ");
static std::shared_ptr<TlsContext> ctx;


class TlsService::Private
{
public:
    inline void save(rapidjson::Document &doc) const    { doc.AddMember(rapidjson::StringRef(TlsConfig::kField), config.toJSON(doc), doc.GetAllocator()); }

    void apply(const TlsConfig &next);
    void create() const;

    TlsConfig config;
};


} // namespace xmrig


xmrig::TlsService::TlsService() :
    d(std::make_shared<Private>())
{
    d->apply(Process::arguments());
}


void xmrig::TlsService::onEvent(uint32_t type, IEvent *event)
{
    if (ConfigEvent::handle(type, event, 0, [this](const IJsonReader &reader, bool valid) {
        if (valid) {
            d->apply({ reader.getValue(TlsConfig::kField), d->config });
        }

        d->create();
    })) {
        return;
    }

    SaveEvent::handle(type, event, 0, [this](rapidjson::Document &doc) { d->save(doc); });
}


void xmrig::TlsService::Private::apply(const TlsConfig &next)
{
    if (config == next) {
        return;
    }

    config = next;
    config.print();
}


void xmrig::TlsService::Private::create() const
{
    try {
        auto next = TlsContext::create(config);
        if (next) {
            ctx = next;
        }
    } catch (std::exception &ex) {
        LOG_ERR("%s " RED_BOLD("%s"), tls_tag(), ex.what());
    }
}


const char *xmrig::tls_tag()
{
    return tag;
}
