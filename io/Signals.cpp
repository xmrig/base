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

#include "base/io/Signals.h"
#include "base/io/log/Log.h"
#include "base/kernel/Events.h"
#include "base/kernel/events/SignalEvent.h"
#include "base/kernel/Process.h"
#include "base/tools/Handle.h"


namespace xmrig {


#ifdef SIGUSR1
static constexpr const int signums[Signals::kSignalsCount] = { SIGHUP, SIGINT, SIGTERM, SIGUSR1 };
#else
static constexpr const int signums[Signals::kSignalsCount] = { SIGHUP, SIGINT, SIGTERM };
#endif


static constexpr const char *signame(int signum)
{
    switch (signum) {
    case SIGHUP:
        return "SIGHUP";

    case SIGINT:
        return "SIGINT";

    case SIGTERM:
        return "SIGTERM";

    default:
        break;
    }

    return nullptr;
}


} // namespace xmrig


xmrig::Signals::Signals()
{
#   ifndef XMRIG_OS_WIN
    signal(SIGPIPE, SIG_IGN);
#   endif

    for (size_t i = 0; i < kSignalsCount; ++i) {
        auto signal  = new uv_signal_t;
        signal->data = this;

        m_signals[i] = signal;

        uv_signal_init(uv_default_loop(), signal);
        uv_signal_start(signal, Signals::onSignal, signums[i]);
    }
}


xmrig::Signals::~Signals()
{
    for (auto signal : m_signals) {
        Handle::close(signal);
    }
}


const char *xmrig::Signals::tag()
{
    static const char *tag = YELLOW_BG_BOLD(WHITE_BOLD_S " signal  ");

    return tag;
}


void xmrig::Signals::onSignal(uv_signal_t * /*handle*/, int signum)
{
    Process::events().send<SignalEvent>(signum);

    switch (signum)
    {
    case SIGHUP:
    case SIGTERM:
    case SIGINT:
        LOG_WARN("%s " YELLOW_BOLD("%s ") YELLOW("received, exiting"), tag(), signame(signum));
        Process::exit(128 + signum);
        break;

    default:
        break;
    }
}
