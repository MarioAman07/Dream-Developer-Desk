// Paths.h - finds the folder that contains shaders/ and assets/: the current
// directory first, then the folder of the executable (same idea as readTextFile
// in the common.h of our assignments), so the program also runs when it is
// started from another directory or by double-clicking the .exe.
#pragma once

#include <string>

namespace paths {

// Call once at startup with argv[0].
void init(const char* argv0);

std::string shader(const std::string& name);   // <root>/shaders/<name>
std::string asset(const std::string& name);    // <root>/assets/<name>

} // namespace paths
