//
// Created by owd on 06/12/24.
//

#ifndef OPTIFOL_EXISTENTIALSHIFTINGVISITOR_HPP
#define OPTIFOL_EXISTENTIALSHIFTINGVISITOR_HPP

#include "../MutatingSentenceVisitorBase.hpp"

namespace optifol
{

class ExistentialShiftingVisitor:
        public MutatingSentenceVisitorBase
{
public:
    void reset() override;
};

}

#endif //OPTIFOL_EXISTENTIALSHIFTINGVISITOR_HPP
