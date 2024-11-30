//
// Created by owd on 30/11/24.
//

#include <cassert>

#include "TermResolutionVisitor.hpp"
#include "../../SemanticException.hpp"

namespace optifol
{

void TermResolutionVisitor::visit(FunctionNode &node)
{
    // TODO
    (void) 0;
}

void TermResolutionVisitor::visit(ConstantNode &node)
{
    // TODO
    (void) 0;
}

void TermResolutionVisitor::visit(VariableNode &node)
{
    const auto &name = node.to_string();

    if (!scope.contains(name))
        throw SemanticException("Referenced variable \"" + name + "\" is not defined in the current scope.");
}

bool TermResolutionVisitor::open_scope(QuantifiedSentenceNode& node)
{
    const auto& original_name = node.get_bound_variable()->to_string();
    bool updated = false;

    if (scope.contains(original_name))
        throw SemanticException("Declared variable \"" + original_name + "\" is already defined in the current scope.");

    if (adjacent.contains(original_name)) {
        auto new_name = generate_name(original_name);

        while (adjacent.contains(new_name))
            new_name = generate_name(new_name);

        node.replace_bound_variable(std::make_shared<VariableNode>(original_name, new_name));
        updated = true;
    }

    scope.emplace(node.get_bound_variable()->to_string());
    return updated;
}

void TermResolutionVisitor::reset()
{
    scope.clear();
}

std::string TermResolutionVisitor::generate_name(const std::string &name)
{
    return name + std::to_string(counter++);
}

void TermResolutionVisitor::close_scope(const VariableNode &node)
{
    const auto number_erased = scope.erase(node.to_string());
    assert(number_erased == 1);

    adjacent.emplace(node.get_disambiguated_name());
}

}
