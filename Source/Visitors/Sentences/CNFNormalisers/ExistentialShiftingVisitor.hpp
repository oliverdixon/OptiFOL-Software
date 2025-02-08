/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */



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
