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

#include "base/kernel/services/DnsService.h"
#include "base/kernel/events/ConfigEvent.h"
#include "base/kernel/events/SaveEvent.h"
#include "base/kernel/private/DnsConfig.h"
#include "base/kernel/Process.h"


namespace xmrig {


static DnsConfig config;


class DnsService::Private
{
public:
    static inline void save(rapidjson::Document &doc)   { doc.AddMember(rapidjson::StringRef(DnsConfig::kField), config.toJSON(doc), doc.GetAllocator()); }

    static void apply(const DnsConfig &next);
};


const DnsConfig &DnsConfig::current()
{
    return config;
}


} // namespace xmrig


xmrig::DnsService::DnsService()
{
    Private::apply(Process::arguments());
}


void xmrig::DnsService::onEvent(uint32_t type, IEvent *event)
{
    if (type == IEvent::CONFIG && event->data() == 0 && !event->isRejected()) {
        return Private::apply({ static_cast<const ConfigEvent *>(event)->reader()->getObject(DnsConfig::kField), config });
    }

    if (type == IEvent::SAVE && event->data() == 0) {
        return Private::save(static_cast<SaveEvent *>(event)->doc());
    }
}


void xmrig::DnsService::Private::apply(const DnsConfig &next)
{
    if (config == next) {
        return;
    }

    config = next;
    config.print();
}
