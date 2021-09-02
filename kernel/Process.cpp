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

#include <ctime>
#include <string>
#include <uv.h>


#include "base/kernel/Process.h"
#include "3rdparty/fmt/core.h"
#include "base/kernel/Versions.h"
#include "base/tools/Arguments.h"
#include "base/tools/Chrono.h"
#include "version.h"


#ifdef XMRIG_OS_WIN
#   ifdef _MSC_VER
#       include <direct.h>
#       define MKDIR(path) _mkdir(path.c_str());
#   else
#       define MKDIR(path) mkdir((path).c_str());
#   endif
#else
#   define MKDIR(path) mkdir(path.c_str(), 0700);
#endif


namespace xmrig {


class ProcessPrivate
{
public:
    ProcessPrivate(int argc, char **argv) : arguments(argc, argv) {}

    void setDataDir(const char *path)
    {
        if (path == nullptr) {
            return;
        }

        std::string dir = path;
        if (!dir.empty() && (dir.back() == '/' || dir.back() == '\\')) {
            dir.pop_back();
        }

        if (!dir.empty() && uv_chdir(dir.c_str()) == 0) {
            dataDir = { dir.data(), dir.size() };
        }
    }

    Arguments arguments;
    const char *version = APP_VERSION;
    String dataDir;
    Versions versions;
};


ProcessPrivate *Process::d_ptr  = nullptr;


} // namespace xmrig


xmrig::Process::Process(int argc, char **argv)
{
    d_ptr = new ProcessPrivate(argc, argv);

    srand(static_cast<unsigned int>(Chrono::currentMSecsSinceEpoch() ^ reinterpret_cast<uintptr_t>(this)));

    d_ptr->setDataDir(arguments().value("--data-dir", "-d"));

#   ifdef XMRIG_SHARED_DATADIR
    if (d_ptr->dataDir.isEmpty()) {
        auto dataDir = fmt::format("{}" XMRIG_DIR_SEPARATOR ".xmrig" XMRIG_DIR_SEPARATOR, locate(HomeLocation));
        MKDIR(dataDir);

        dataDir += APP_KIND;
        MKDIR(dataDir);

        if (uv_chdir(dataDir.c_str()) == 0) {
            d_ptr->dataDir = { dataDir.c_str(), dataDir.size() };
        }
    }
#   endif

    if (d_ptr->dataDir.isEmpty()) {
        d_ptr->dataDir = locate(ExeLocation);
    }
}


xmrig::Process::~Process()
{
    delete d_ptr;
}


const xmrig::Arguments &xmrig::Process::arguments()
{
    return d_ptr->arguments;
}


const char *xmrig::Process::version()
{
    return d_ptr->version;
}


const xmrig::Versions &xmrig::Process::versions()
{
    return d_ptr->versions;
}


int xmrig::Process::ppid()
{
#   if UV_VERSION_HEX >= 0x011000
    return uv_os_getppid();
#   else
    return 0;
#   endif
}


xmrig::String xmrig::Process::locate(Location location, const char *fileName)
{
    auto path = locate(location);
    if (path.isNull() || fileName == nullptr) {
        return path;
    }

    return fmt::format("{}" XMRIG_DIR_SEPARATOR "{}", path, fileName).c_str();
}


xmrig::String xmrig::Process::locate(Location location)
{
    char buf[520]{};
    size_t size = sizeof(buf);

    if (location == ExePathLocation && uv_exepath(buf, &size) >= 0) {
        return { buf, size };
    }

    if (location == Process::DataLocation && !d_ptr->dataDir.isEmpty()) {
        return d_ptr->dataDir;
    }

#   if UV_VERSION_HEX >= 0x010600
    if (location == Process::HomeLocation && uv_os_homedir(buf, &size) >= 0) {
        return { buf, size };
    }
#   endif

#   if UV_VERSION_HEX >= 0x010900
    if (location == Process::TempLocation && uv_os_tmpdir(buf, &size) >= 0) {
        return { buf, size };
    }
#   endif

    if (location == Process::CwdLocation && uv_cwd(buf, &size) >= 0) {
        return { buf, size };
    }

    if (location == Process::ExeLocation) {
        if (uv_exepath(buf, &size) < 0) {
            return {};
        }

        auto path       = std::string(buf, size);
        const auto pos  = path.rfind(*XMRIG_DIR_SEPARATOR);

        if (pos != std::string::npos) {
            return path.substr(0, pos).c_str();
        }

        return { buf, size };
    }

    return location != ExeLocation ? locate(ExeLocation) : String();
}
