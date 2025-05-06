/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Quantifier Extracting Visitor
 * @author Oliver Dixon
 * @date 2025-05-04
 * @version Development
 */

#ifndef OPTIFOL_QUANTIFIEREXTRACTINGVISITOR_HPP
#define OPTIFOL_QUANTIFIEREXTRACTINGVISITOR_HPP

#include "../MutatingSentenceVisitorBase.hpp"

namespace optifol
{

class QuantifierExtractingVisitor:
        public MutatingSentenceVisitorBase
{
public:
    void reset() override;
};

}

#endif
