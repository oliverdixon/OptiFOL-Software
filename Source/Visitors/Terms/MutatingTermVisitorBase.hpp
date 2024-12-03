/**
 * @file MutatingTermVisitorBase.hpp
 * @brief Class specification for the Mutating Term Visitor base class.
 * @author Oliver Dixon
 * @date 2024-11-30
 * @version Development
 */

#ifndef OPTIFOL_MUTATINGTERMVISITORBASE_HPP
#define OPTIFOL_MUTATINGTERMVISITORBASE_HPP

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

#endif //OPTIFOL_MUTATINGTERMVISITORBASE_HPP
