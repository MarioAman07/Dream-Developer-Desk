#include "core/Paths.h"

#include <fstream>
#include <iostream>

namespace {
std::string g_root = ".";

// A folder is the project folder if shaders/scene.vert can be opened from it.
bool looksLikeRoot(const std::string& dir)
{
    std::ifstream file(dir + "/shaders/scene.vert");
    return file.is_open();
}
} // namespace

namespace paths {

void init(const char* argv0)
{
    // 1) The current directory (the program is normally started from the project folder).
    if (looksLikeRoot(".")) {
        g_root = ".";
        return;
    }

    // 2) The folder of the executable.
    if (argv0) {
        std::string exePath(argv0);
        std::string::size_type slash = exePath.find_last_of("/\\");
        if (slash != std::string::npos && looksLikeRoot(exePath.substr(0, slash))) {
            g_root = exePath.substr(0, slash);
            std::cout << "[Paths] Using resources from " << g_root << "\n";
            return;
        }
    }
    std::cerr << "[Paths] Could not find shaders/ and assets/ - run the program from the project folder.\n";
}

std::string shader(const std::string& name) { return g_root + "/shaders/" + name; }
std::string asset(const std::string& name) { return g_root + "/assets/" + name; }

} // namespace paths
