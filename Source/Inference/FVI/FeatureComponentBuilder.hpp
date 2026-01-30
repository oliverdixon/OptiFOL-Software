/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 1/30/26.
//

#ifndef OPTIFOL_FEATURECOMPONENTBUILDER_HPP
#define OPTIFOL_FEATURECOMPONENTBUILDER_HPP

#include "../../Optifol.hpp"
#include "OccurrenceCountFeature.hpp"

namespace optifol
{

class Clause;
class Function;
class SkolemFunction;
class Variable;
class Constant;
class Literal;
class IProcessedTerm;

class FeatureComponentBuilder
{
public:
    using OCMapType = std::unordered_map<OccurrenceCountFeature, unsigned int, std::hash<OccurrenceCountFeature>,
        std::equal_to<>>;

    explicit FeatureComponentBuilder(std::add_lvalue_reference_t<OCMapType> destination_oc, bool is_positive);

    void visit(const Literal *literal) const;

    void visit(const Function *function) const;

    void visit(const SkolemFunction * skolem_function) const;

    void visit(const Variable *variable) const;

    void visit(const Constant *constant) const;

private:
    bool is_positive;

    void mark_occurrence(const Literal *literal) const;

    void mark_occurrence(const IProcessedTerm *term) const;

    std::add_lvalue_reference_t<OCMapType> destination_oc;
};

} // namespace optifol

#endif // OPTIFOL_FEATURECOMPONENTBUILDER_HPP
