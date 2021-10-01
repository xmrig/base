/* XMRig
 * Copyright (c) 2016      Ivan Vizir  <define-true-false@yandex.com>
 * Copyright (c) 2016      The Qt Company Ltd.
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

#include "base/kernel/Taskbar.h"


#include <algorithm>
#include <cmath>
#include <Objbase.h>
#include <Shobjidl.h>


namespace xmrig {


class Taskbar::Private
{
public:
    void update();

    bool paused             = false;
    bool stopped            = false;
    bool visible            = false;
    HWND handle             = nullptr;
    int maximum             = 100;
    int minimum             = 0;
    int value               = 0;
    ITaskbarList3 *taskbar  = nullptr;

private:
    TBPFLAG state() const;
};


} // namespace xmrig


xmrig::Taskbar::Taskbar() :
    d(std::make_shared<Private>())
{
    if (FAILED(CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&d->taskbar)))) {
        return;
    }

    if (FAILED(d->taskbar->HrInit())) {
        d->taskbar->Release();
        d->taskbar = nullptr;

        return;
    }

    d->handle = GetConsoleWindow();
}


xmrig::Taskbar::~Taskbar()
{
    if (d->taskbar) {
        d->taskbar->Release();
    }
}


bool xmrig::Taskbar::isPaused() const
{
    return d->paused;
}


bool xmrig::Taskbar::isStopped() const
{
    return d->stopped;
}


bool xmrig::Taskbar::isVisible() const
{
    return d->visible;
}


int xmrig::Taskbar::maximum() const
{
    return d->maximum;
}


int xmrig::Taskbar::minimum() const
{
    return d->minimum;
}


int xmrig::Taskbar::value() const
{
    return d->value;
}


void xmrig::Taskbar::resume()
{
    setPaused(false);

    if (d->stopped) {
        d->stopped = false;
        d->update();
    }
}


void xmrig::Taskbar::setMaximum(int maximum)
{
    setRange(std::min(d->minimum, maximum), maximum);
}


void xmrig::Taskbar::setMinimum(int minimum)
{
    setRange(minimum, std::max(minimum, d->maximum));
}


void xmrig::Taskbar::setPaused(bool paused)
{
    if (paused == d->paused || d->stopped) {
        return;
    }

    d->paused = paused;
    d->update();
}


void xmrig::Taskbar::setRange(int minimum, int maximum)
{
    if (minimum != d->minimum || maximum != d->maximum) {
        d->minimum = minimum;
        d->maximum = std::max(minimum, maximum);

        if (d->value < d->minimum || d->value > d->maximum) {
            reset();
        }

        d->update();
    }
}


void xmrig::Taskbar::setValue(int value)
{
    if ((value == d->value) || value < d->minimum || value > d->maximum) {
        return;
    }

    d->value = value;
    d->update();
}


void xmrig::Taskbar::setVisible(bool visible)
{
    if (visible == d->visible) {
        return;
    }

    d->visible = visible;
    d->update();
}


void xmrig::Taskbar::stop()
{
    setPaused(false);

    if (!d->stopped) {
        d->stopped = true;
        d->update();
    }
}


void xmrig::Taskbar::Private::update()
{
    if (!taskbar) {
        return;
    }

    const int range = maximum - minimum;

    if (range > 0) {
        taskbar->SetProgressValue(handle, lround(double(100) * (double(value - minimum)) / double(range)), 100);
    }

    taskbar->SetProgressState(handle, state());
}


TBPFLAG xmrig::Taskbar::Private::state() const
{
    if (!taskbar || !visible) {
        return TBPF_NOPROGRESS;
    }

    if (stopped) {
        return TBPF_ERROR;
    }

    if (paused) {
        return TBPF_PAUSED;
    }

    if (minimum == 0 && maximum == 0) {
        return TBPF_INDETERMINATE;
    }

    return TBPF_NORMAL;
}
