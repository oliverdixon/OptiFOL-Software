/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification and implementation for the Semantic Exception
 * @author Oliver Dixon
 * @date 2024-11-30
 * @version Development
 */

#ifndef SEMANTICEXCEPTION_HPP
#define SEMANTICEXCEPTION_HPP

#include <stdexcept>

namespace optifol
{

/**
 * @class SemanticException
 * @brief The Semantic Exception expresses a logical/semantic error in the meaning of a Requirement statement.
 * @details The Semantic Exception can be used to express errors of a nature that, while syntactically unproblematic,
 *  do not make meaningful sense in the present context. The most canonical usage would be to indicate a logical
 *  inconsistency in a requirement sentence according to the rules of FOL. See, for example, SymbolStandardisingVisitor.
 */
class SemanticException final :
        public std::runtime_error
{
public:
    explicit SemanticException(const std::string& message):
        std::runtime_error(std::string("Semantic Exception: ") + message)
    { }

    explicit SemanticException(const char* message):
        std::runtime_error(std::string("Semantic Exception: ") + message)
    { }
};

}

#endif
