/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */



#ifndef OPTIFOL_DISABLE_GUI

#include "Application.hpp"

int main(int argc, char **argv)
{
    auto app = optifol::Application::create();
    return app->run(argc, argv);
}

#endif
