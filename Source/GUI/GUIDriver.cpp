/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief OptiFOL GUI entry point definition
 * @author Oliver Dixon
 * @date 2025-02-02
 * @version Development
 */

#ifndef OPTIFOL_DISABLE_GUI

#include "Application.hpp"

int main(const int argc, char **argv)
{
    const auto app = optifol::Application::create();
    return app->run(argc, argv);
}

#endif
