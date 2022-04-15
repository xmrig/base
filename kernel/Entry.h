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

#ifndef XMRIG_ENTRY_H
#define XMRIG_ENTRY_H


#include "base/tools/Object.h"


#include <functional>
#include <string>
#include <vector>


namespace xmrig {


class Entry
{
public:
    XMRIG_DISABLE_COPY_MOVE(Entry)

    using Fn    = std::function<bool(int &rc)>;
    using Usage = std::function<std::string()>;

    Entry(const Usage &usage);

    bool exec(int &rc) const;
    void add(Fn &&fn);

private:
    static bool background(int &rc);

    std::vector<Fn> m_entries;
};


} // namespace xmrig


#endif // XMRIG_ENTRY_H
