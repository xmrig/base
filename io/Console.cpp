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

#include "base/io/Console.h"
#include "base/io/log/Log.h"
#include "base/io/Signals.h"
#include "base/kernel/Events.h"
#include "base/kernel/events/ConsoleEvent.h"
#include "base/kernel/private/Title.h"
#include "base/kernel/Process.h"
#include "base/tools/Cvt.h"
#include "base/tools/Handle.h"


namespace xmrig {


class Console::Private
{
public:
    XMRIG_DISABLE_COPY_MOVE(Private)

    Private();
    ~Private();

#   ifdef XMRIG_OS_WIN
    std::wstring title;
#   endif

private:
    static bool isSupported();

    static void onAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
    static void onRead(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);

    char m_buf[1] = { 0 };
    uv_tty_t *m_tty = nullptr;
};


} // namespace xmrig


xmrig::Console::Console() :
    d(std::make_shared<Private>())
{
}


#ifdef XMRIG_OS_WIN
void xmrig::Console::setTitle(const Title &title) const
{
    SetConsoleTitleW(title.isEnabled() ? Cvt::toUtf16(title.value()).c_str() : d->title.c_str());
}
#endif


xmrig::Console::Private::Private()
{
#   ifdef XMRIG_OS_WIN
    {
        constexpr size_t kMaxTitleLength = 8192;

        wchar_t title_w[kMaxTitleLength]{};
        title = { title_w, GetConsoleTitleW(title_w, kMaxTitleLength) };
    }
#   endif

    if (!isSupported()) {
        return;
    }

    m_tty = new uv_tty_t;
    m_tty->data = this;
    uv_tty_init(uv_default_loop(), m_tty, 0, 1);

    if (!uv_is_readable(reinterpret_cast<uv_stream_t*>(m_tty))) {
        return;
    }

    uv_tty_set_mode(m_tty, UV_TTY_MODE_RAW);
    uv_read_start(reinterpret_cast<uv_stream_t*>(m_tty), onAllocBuffer, onRead);
}


xmrig::Console::Private::~Private()
{
    uv_tty_reset_mode();

    Handle::close(m_tty);
}


bool xmrig::Console::Private::isSupported()
{
    const uv_handle_type type = uv_guess_handle(0);
    return type == UV_TTY || type == UV_NAMED_PIPE;
}


void xmrig::Console::Private::onAllocBuffer(uv_handle_t *handle, size_t, uv_buf_t *buf)
{
    auto console = static_cast<Private *>(handle->data);
    buf->len  = 1;
    buf->base = console->m_buf;
}


void xmrig::Console::Private::onRead(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    if (nread < 0) {
        return uv_close(reinterpret_cast<uv_handle_t*>(stream), nullptr);
    }

    if (nread == 1) {
        const char command = *buf->base;
        if (*buf->base == 3) {
            LOG_WARN("%s " YELLOW_BOLD("Ctrl+C ") YELLOW("received, exiting"), Signals::tag());

            Process::exit(0);
        }
        else {
            Process::events().send<ConsoleEvent>(command);
        }
    }
}
