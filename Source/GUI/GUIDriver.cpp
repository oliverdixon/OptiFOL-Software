//
// Created by owd on 07/12/24.
//

#ifdef OPTIFOL_GUI

#include "Application.hpp"

int main(int argc, char **argv)
{
    auto app = optifol::Application::create();
    return app->run(argc, argv);
}

#endif
