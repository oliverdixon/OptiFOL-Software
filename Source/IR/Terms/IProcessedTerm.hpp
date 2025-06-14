/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 6/14/25.
//

#ifndef IPROCESSEDTERM_HPP
#define IPROCESSEDTERM_HPP

#include "ITerm.hpp"

namespace optifol {

class IProcessedTerm :
        public ITerm
{
public:
    virtual bool accept(UnificationVisitor &visitor, const IProcessedTerm &target) const;

    virtual bool accept(UnificationVisitor &visitor, const Function &target) const;
};

} // optifol

#endif //IPROCESSEDTERM_HPP
