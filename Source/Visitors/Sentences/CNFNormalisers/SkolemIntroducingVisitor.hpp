//
// Created by owd on 06/12/24.
//

#ifndef OPTIFOL_SKOLEMINTRODUCINGVISITOR_HPP
#define OPTIFOL_SKOLEMINTRODUCINGVISITOR_HPP

#include "../MutatingSentenceVisitorBase.hpp"

namespace optifol
{

class SkolemIntroducingVisitor:
        public MutatingSentenceVisitorBase
{
public:
    void reset() override;
};

}

#endif //OPTIFOL_SKOLEMINTRODUCINGVISITOR_HPP
