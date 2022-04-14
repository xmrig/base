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

#include "base/kernel/events/ConsoleEvent.h"
#include "base/io/log/Log.h"


namespace xmrig {


static const char *kTag = MAGENTA_BG(WHITE_BOLD_S " console ");


} // namespace xmrig


const char *xmrig::ConsoleEvent::tag() const
{
    return kTag;
}


void xmrig::ConsoleEvent::help(const char *key, const char *text)
{
    LOG_NOTICE("%s " MAGENTA_BOLD("%-6s") " - %s", kTag, key, text);
}


#ifdef APP_DEBUG
void xmrig::ConsoleEvent::print() const
{
    LOG_DEBUG("%s " MAGENTA("<command=") CYAN("0x%x") MAGENTA(">"), tag(), command());
}
#endif
