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

class MutableFunctionNode;
class MutableConstantNode;
class MutableVariableNode;
class MutableSkolemFunctionNode;

class MutatingTermVisitorBase
{
public:
    virtual ~MutatingTermVisitorBase() = default;

    virtual void visit(MutableFunctionNode &node);

    virtual void visit(MutableConstantNode &node);

    virtual void visit(MutableVariableNode &node);

    virtual void visit(MutableSkolemFunctionNode& node);
};

}

#endif
