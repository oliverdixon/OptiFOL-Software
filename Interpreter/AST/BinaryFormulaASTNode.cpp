//
// Created by owd on 20/11/24.
//

#include "BinaryFormulaASTNode.hpp"

namespace optifol
{

template<> const char *BinaryFormulaASTNode<BinaryFormulaASTTypes::Conjunction>::operator_symbol() { return "Conj"; }

template<> const char *BinaryFormulaASTNode<BinaryFormulaASTTypes::Disjunction>::operator_symbol() { return "Disj"; }

template<> const char *BinaryFormulaASTNode<BinaryFormulaASTTypes::Implication>::operator_symbol() { return "Impl"; }

template<> const char *BinaryFormulaASTNode<BinaryFormulaASTTypes::Biconditional>::operator_symbol() { return "Bcnd"; }

}
