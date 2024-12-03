/**
 * @file SemanticException.hpp
 * @brief Class definition for the Semantic Exception
 * @author Oliver Dixon
 * @date 2024-11-30
 * @version Development
 */

#ifndef OPTIFOL_SEMANTICEXCEPTION_HPP
#define OPTIFOL_SEMANTICEXCEPTION_HPP

#include <stdexcept>

namespace optifol
{

class SemanticException :
        public std::runtime_error
{
public:
    template<class... Args>
    explicit SemanticException(Args &&... args):
            std::runtime_error(std::forward<Args>(args)...)
    {}
};

}

#endif //OPTIFOL_SEMANTICEXCEPTION_HPP
