// Dream Developer Desk - Computer Graphics Fundamentals final project (topic 28).
// Entry point: finds the folder with shaders/ and assets/ and starts the application.
#include "core/App.h"
#include "core/Paths.h"

int main(int argc, char** argv)
{
    paths::init(argc > 0 ? argv[0] : nullptr);
    App app;
    return app.run();
}
