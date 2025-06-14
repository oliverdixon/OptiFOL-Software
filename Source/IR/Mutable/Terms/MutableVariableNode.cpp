/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the mutable Variable Term IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#include "MutableVariableNode.hpp"

#include "../../../Visitors/Terms/MutatingTermVisitorBase.hpp"
#include "../../../Visitors/Unification/UnificationVisitor.hpp"
#include "MutableFunctionNode.hpp"

namespace optifol
{

MutableVariableNode::MutableVariableNode(std::string name):
    name(std::move(name))
{}

MutableVariableNode::MutableVariableNode(std::string name, const std::string &disambiguated_name) :
    name(std::move(name)),
    disambiguated_name(disambiguated_name)
{}

std::unique_ptr<IMutableTermNode> MutableVariableNode::clone() const
{
    if (disambiguated_name.has_value())
        return std::make_unique<MutableVariableNode>(name, *disambiguated_name);

    return std::make_unique<MutableVariableNode>(name);
}

std::string MutableVariableNode::to_string() const
{
    return name;
}

std::string MutableVariableNode::get_disambiguated_name() const
{
    return disambiguated_name.value_or(name);
}

void MutableVariableNode::accept(MutatingTermVisitorBase &visitor)
{
    visitor.visit(*this);
}

bool MutableVariableNode::accept(UnificationVisitor &visitor, const IMutableTermNode &target) const
{
    return visitor.visit(*this, target);
}

bool MutableVariableNode::accept(UnificationVisitor &visitor, const MutableVariableNode &target) const
{
    return visitor.visit(*this, target);
}

std::ostream &MutableVariableNode::serialise(std::ostream &ostream) const
{
    return ostream << name;
}

}
