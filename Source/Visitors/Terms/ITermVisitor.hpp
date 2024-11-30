//
// Created by owd on 30/11/24.
//

#ifndef OPTIFOL_ITERMVISITOR_HPP
#define OPTIFOL_ITERMVISITOR_HPP

namespace optifol
{

class FunctionNode;
class ConstantNode;
class VariableNode;

class ITermVisitor
{
public:
    virtual void visit(FunctionNode& node) = 0;

    virtual void visit(ConstantNode& node) = 0;

    virtual void visit(VariableNode& node) = 0;

    virtual void reset() = 0;
};

}

#endif //OPTIFOL_ITERMVISITOR_HPP
