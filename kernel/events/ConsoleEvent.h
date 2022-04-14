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

#ifndef XMRIG_CONSOLEEVENT_H
#define XMRIG_CONSOLEEVENT_H


#include "base/kernel/events/Event.h"


namespace xmrig {


class ConsoleEvent : public Event
{
public:
    XMRIG_DISABLE_COPY_MOVE_DEFAULT(ConsoleEvent)

    inline ConsoleEvent(char command) : m_command(command)  {}
    inline ~ConsoleEvent() override = default;

    inline char command() const     { return m_command; }

    template<typename... Args>
    static inline bool handle(uint32_t type, IEvent *event, Args... args)
    {
        if (type == CONSOLE) {
            for (const char key : { args... }) {
                if (static_cast<ConsoleEvent *>(event)->command() == key) {
                    return true;
                }
            }
        }

        return false;
    }

    const char *tag() const override;

    static void help(const char *key, const char *text);

protected:
    uint32_t type() const override  { return CONSOLE; }
    uint64_t data() const override  { return static_cast<uint64_t>(m_command); }

#   ifdef APP_DEBUG
    void print() const override;
#   endif

private:
    const char m_command;
};


} // namespace xmrig


#endif // XMRIG_CONSOLEEVENT_H
