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

#ifndef XMRIG_TLSSERVICE_H
#define XMRIG_TLSSERVICE_H


#include "base/kernel/Service.h"


namespace xmrig {


class TlsService : public Service
{
public:
    XMRIG_DISABLE_COPY_MOVE(TlsService)

    TlsService();
    ~TlsService() override = default;

protected:
    void onEvent(uint32_t type, IEvent *event) override;

private:
    XMRIG_DECL_PRIVATE()
};


const char *tls_tag();


} // namespace xmrig


#endif // XMRIG_TLSSERVICE_H
