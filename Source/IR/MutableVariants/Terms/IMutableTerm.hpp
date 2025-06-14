/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the generic mutable IR Term interface
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef IMUTABLETERM_HPP
#define IMUTABLETERM_HPP

#include <memory>
#include <vector>

#include "../../Terms/ITerm.hpp"

namespace optifol
{

class MutableFunction;
class MutableVariable;
class UnificationVisitor;
class MutatingTermVisitorBase;

class IMutableTerm :
        public ITerm
{
public:
    [[nodiscard]] virtual std::unique_ptr<IMutableTerm> clone() const = 0;

    virtual void accept(MutatingTermVisitorBase &visitor) = 0;

private:
    std::vector<std::reference_wrapper<const MutableVariable>> substitution_keys;
};

}

#endif
