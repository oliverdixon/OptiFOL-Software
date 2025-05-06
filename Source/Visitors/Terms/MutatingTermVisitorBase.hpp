/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Mutating Term Visitor base class.
 * @author Oliver Dixon
 * @date 2024-11-30
 * @version Development
 */

#ifndef MUTATINGTERMVISITORBASE_HPP
#define MUTATINGTERMVISITORBASE_HPP

namespace optifol
{

class FunctionNode;
class ConstantNode;
class VariableNode;

class MutatingTermVisitorBase
{
public:
    virtual ~MutatingTermVisitorBase() = default;

    virtual void visit(FunctionNode &node);

    virtual void visit(ConstantNode &node);

    virtual void visit(VariableNode &node);
};

}

#endif
