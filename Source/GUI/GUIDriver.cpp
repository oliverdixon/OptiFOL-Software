/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Optifol GUI entry point definition
 * @author Oliver Dixon
 * @date 2025-02-02
 * @version Development
 */

#include <log4cxx/basicconfigurator.h>
#include "Application.hpp"

int main(const int argc, char **argv)
{
    log4cxx::BasicConfigurator::configure();
    const auto app = optifol::Application::create();
    return app->run(argc, argv);
}
