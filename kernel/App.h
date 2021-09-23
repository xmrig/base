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

#ifndef XMRIG_APP_H
#define XMRIG_APP_H


#include "base/tools/Object.h"


#include <memory>


namespace xmrig {


class AppPrivate;
class Service;


class App
{
public:
    XMRIG_DISABLE_COPY_MOVE(App)

    App();
    ~App();

    template<typename T, typename... Args>
    inline T *add(Args&&... args) {
        auto *service = new T(std::forward<Args>(args)...);
        add(service);

        return service;
    }

    static const char *tag();

    int exec();
    void add(Service *service);
    void remove(Service *service);

private:
    XMRIG_DECL_PRIVATE()
};


} // namespace xmrig


#endif // XMRIG_APP_H
