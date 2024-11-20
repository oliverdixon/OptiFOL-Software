//
// Created by owd on 20/11/24.
//

#include "UnaryFormulaASTNode.hpp"

namespace optifol
{

template<> const char *UnaryFormulaASTNode<UnaryFormulaASTTypes::Negation>::operator_symbol() { return "Negn"; }

}
