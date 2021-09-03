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

#include "base/kernel/Entry.h"
#include "base/io/log/Log.h"
#include "base/kernel/Process.h"
#include "base/kernel/Versions.h"
#include "base/tools/Arguments.h"
#include "version.h"


#include <iostream>


#ifdef XMRIG_FEATURE_HWLOC
#   include <hwloc.h>
#endif


namespace xmrig {


static bool showVersion(int &/*rc*/)
{
    if (Process::arguments().contains("-V", "--version")) {
        std::cout << "v" << Process::version() << std::endl;

        return true;
    }

    return false;
}


static bool showVersions(int &/*rc*/)
{
    if (Process::arguments().contains("--versions")) {
        for (const auto &kv : Process::versions().get()) {
            std::cout << kv.first << "/" << kv.second << "\n";
        }

        return true;
    }

    return false;
}


#ifdef XMRIG_FEATURE_HWLOC
static bool exportTopology(int &rc)
{
    if (!Process::arguments().contains("--export-topology")) {
        return false;
    }

    const auto path = Process::locate(Process::DataLocation, "topology.xml");

    hwloc_topology_t topology = nullptr;
    hwloc_topology_init(&topology);
    hwloc_topology_load(topology);

#   if HWLOC_API_VERSION >= 0x20000
    if (hwloc_topology_export_xml(topology, path, 0) == -1) {
#   else
    if (hwloc_topology_export_xml(topology, path) == -1) {
#   endif
        rc = 1;
        std::cout << "failed to export hwloc topology" << std::endl;
    }
    else {
        std::cout << "hwloc topology successfully exported to \"" << path << '"' << std::endl;
    }

    hwloc_topology_destroy(topology);

    return true;
}
#endif


} // namespace xmrig


xmrig::Entry::Entry(const Usage &usage)
{
    add(showVersion);
    add(showVersions);

#   ifdef XMRIG_FEATURE_HWLOC
    add(exportTopology);
#   endif

    add([usage](int &/*rc*/) {
        if (!Process::arguments().contains("-h", "--help")) {
            return false;
        }

        std::cout << "Usage: " APP_ID " [OPTIONS]\n";
        std::cout << usage();
        std::cout << "\nBase:\n";
        std::cout << "  -h, --help                    print this help and exit\n";
        std::cout << "  -V, --version                 print " APP_ID " version and exit\n";
        std::cout << "      --versions                print versions and exit\n";
        std::cout << "  -d, --data-dir=<PATH>         specify an alternative working directory\n";
        std::cout << "  -B, --background              run " APP_ID " in the background\n";

#       ifdef XMRIG_FEATURE_HWLOC
        std::cout << "      --export-topology         export hwloc topology to a XML file and exit\n";
#       endif

        return true;
    });

    add([](int &rc) {
        if (Process::arguments().contains("-B", "--background")) {
            Log::setBackground(true);

            return background(rc);
        }

        return false;
    });
}


bool xmrig::Entry::exec(int &rc) const
{
    for (const auto &fn : m_entries) {
        if (fn(rc)) {
            return true;
        }
    }

    return false;
}


void xmrig::Entry::add(Fn &&fn)
{
    m_entries.emplace_back(std::move(fn));
}
