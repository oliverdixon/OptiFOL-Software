/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the Semantic Exception
 * @author Oliver Dixon
 * @date 2025-02-09
 * @version Development
 */

#include "SemanticException.hpp"

namespace optifol
{

const std::string SemanticException::prefix = "Semantic Exception: ";

SemanticException::SemanticException(const std::string &message):
    ExceptionBase(prefix + message)
{
}

SemanticException::SemanticException(const char *message):
    ExceptionBase(prefix + message)
{
}

}
