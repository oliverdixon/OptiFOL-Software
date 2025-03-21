/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the statically available log4cxx facilities
 * @author Oliver Dixon
 * @date 2025-03-21
 * @version Development
 */

#include <log4cxx/basicconfigurator.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/propertyconfigurator.h>

#include "Logging.hpp"

namespace optifol
{

Logging::LifecycleManager Logging::manager;

const char * const Logging::LifecycleManager::properties_file = "optifol.log4cxx.properties";

log4cxx::LoggerPtr Logging::get_logger()
{
    return log4cxx::LogManager::getRootLogger();
}

log4cxx::LoggerPtr Logging::get_logger(const std::string &name)
{
    return log4cxx::LogManager::getLogger(name);
}

Logging::LifecycleManager::LifecycleManager()
{
    if (log4cxx::PropertyConfigurator::configure(properties_file) ==
            log4cxx::spi::ConfigurationStatus::NotConfigured) {
        // If we couldn't load the custom configurator, send events to the console.
        log4cxx::BasicConfigurator::configure();
        LOG4CXX_WARN(get_logger(), "Could not load logging properties file at " << properties_file);
    }
}

Logging::LifecycleManager::~LifecycleManager()
{
    log4cxx::LogManager::shutdown();
}

}
