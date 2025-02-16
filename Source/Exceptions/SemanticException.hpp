/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class definition for the Semantic Exception
 * @author Oliver Dixon
 * @date 2024-11-30
 * @version Development
 */

#ifndef OPTIFOL_SEMANTICEXCEPTION_HPP
#define OPTIFOL_SEMANTICEXCEPTION_HPP

#include "ExceptionBase.hpp"

namespace optifol
{

/**
 * @class SemanticException
 * @brief The Semantic Exception expresses a logical/semantic error in the meaning of a requirement statement.
 *
 * @details The Semantic Exception can be used to express errors of a nature that, while syntactically unproblematic,
 *  do not make meaningful sense in the present context. The most canonical usage would be to indicate a logical
 *  inconsistency in a requirement sentence according to the rules of FOL. See, for example, SymbolStandardisingVisitor.
 */
class SemanticException :
        public ExceptionBase
{
public:
    /**
     * @copydoc ExceptionBase::ExceptionBase(const std::string&)
     */
    explicit SemanticException(const std::string& message);

    /**
     * @copydoc ExceptionBase::ExceptionBase(const char *)
     */
    explicit SemanticException(const char* message);

private:
    /**
     * @brief The prefix string to prepend to all SemanticException messages
     */
    static const std::string prefix;
};

}

#endif
