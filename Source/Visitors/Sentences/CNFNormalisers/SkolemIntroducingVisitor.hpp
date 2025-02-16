/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */



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

#endif
