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

/**
 * @class Logging
 * @brief Provide log4cxx infrastructure from a static context.
 */
class Logging
{
public:
    /**
     * @brief Get the root logger
     * @return A shared pointer to the root log4cxx logger
     */
    static log4cxx::LoggerPtr get_logger();

    /**
     * @brief Get the named logger
     * @param name The period-delimited qualified name of the log4cxx logger to retrieve
     * @return A shared pointer to the named log4cxx logger
     */
    static log4cxx::LoggerPtr get_logger(const char *name);

    /**
     * @brief Get the named logger
     * @param name The field-delimited qualified name of the log4cxx logger to retrieve, with one element per
     *  index
     * @return A shared pointer to the named log4cxx logger
     */
    static log4cxx::LoggerPtr get_logger(std::vector<std::string> &&name);

private:
    static struct LifecycleManager
    {
        LifecycleManager();
        ~LifecycleManager();

    private:
        static const char *const properties_file;
    } manager;
};

} // namespace optifol

#endif
