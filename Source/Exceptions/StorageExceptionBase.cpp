/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Implementation of base class for storage exceptions
 * @author Oliver Dixon
 * @date 2025-02-16
 * @version Development
 */

#include "StorageExceptionBase.hpp"

namespace optifol
{

const std::string StorageExceptionBase::prefix = "Storage Error / ";

StorageExceptionBase::StorageExceptionBase(const std::string &message):
    ExceptionBase(prefix + message)
{
}

StorageExceptionBase::StorageExceptionBase(const char *message):
    ExceptionBase(prefix + message)
{
}

}
