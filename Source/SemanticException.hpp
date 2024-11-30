//
// Created by owd on 30/11/24.
//

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
