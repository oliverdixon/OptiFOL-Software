/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the statically available log4cxx facilities
 * @author Oliver Dixon
 * @date 2025-03-21
 * @version Development
 */

#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <log4cxx/logger.h>

namespace optifol
{

class Logging
{
public:
    static log4cxx::LoggerPtr get_logger();

    static log4cxx::LoggerPtr get_logger(const char * name);

    static log4cxx::LoggerPtr get_logger(std::vector<std::string> &&name);

private:
    static struct LifecycleManager
    {
        LifecycleManager();
        ~LifecycleManager();

    private:
        static const char * const properties_file;
    } manager;
};

}

#endif
