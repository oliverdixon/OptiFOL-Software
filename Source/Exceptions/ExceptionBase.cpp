/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the base OptiFOL C++ exception
 * @author Oliver Dixon
 * @date 2025-02-09
 * @version Development
 */

#include "ExceptionBase.hpp"

namespace optifol
{

const std::string ExceptionBase::prefix = "OptiFOL Error / ";

ExceptionBase::ExceptionBase(const std::string &message):
    std::runtime_error(prefix + message)
{
}

ExceptionBase::ExceptionBase(const char *message):
    std::runtime_error(prefix + message)
{
}

}
