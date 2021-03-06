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

#ifndef XMRIG_TIMER_H
#define XMRIG_TIMER_H


using uv_timer_t = struct uv_timer_s;


#include "base/tools/Object.h"


namespace xmrig {


class ITimerListener;


class Timer
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(Timer);

    Timer(ITimerListener *listener, uint64_t repeat) : m_listener(listener)                     { init(); start(repeat, repeat); }
    Timer(ITimerListener *listener, uint64_t timeout, uint64_t repeat) : m_listener(listener)   { init(); start(timeout, repeat); }
    Timer(ITimerListener *listener) : m_listener(listener)                                      { init(); }
    ~Timer();

    inline int id() const { return m_id; }

    uint64_t repeat() const;
    void setRepeat(uint64_t repeat);
    void singleShot(uint64_t timeout, int id = 0);
    void start(uint64_t timeout, uint64_t repeat);
    void stop();

private:
    void init();

    static void onTimer(uv_timer_t *handle);

    int m_id                    = 0;
    ITimerListener *m_listener;
    uv_timer_t *m_timer         = nullptr;
};


} // namespace xmrig


#endif // XMRIG_TIMER_H
