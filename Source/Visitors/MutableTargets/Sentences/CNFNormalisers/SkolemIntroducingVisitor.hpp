/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Existential Shifting Visitor
 * @author Oliver Dixon
 * @date 2025-05-04
 * @version Development
 */

#ifndef SKOLEMINTRODUCINGVISITOR_HPP
#define SKOLEMINTRODUCINGVISITOR_HPP

#include <stack>
#include <unordered_map>
#include <vector>

#include "../../../../IR/MutableVariants/Sentences/IMutableSentence.hpp"
#include "../../Terms/TermResolutionVisitor.hpp"
#include "../MutatingSentenceVisitorBase.hpp"

namespace optifol
{

class SkolemIntroducingVisitor:
        public MutatingSentenceVisitorBase
{
public:
    SkolemIntroducingVisitor();

    ~SkolemIntroducingVisitor() override;

    [[nodiscard]] std::string_view get_visitor_name() const override;

    void visit(MutableQuantified &node) override;

    void visit(MutablePredicate &node) override;

    void visit(MutableIdentity &node) override;

    void visit(MutableBinaryConnected &node) override;

    void visit(MutableSentenceRoot &node) override;

private:
    void open_scope(MutableQuantified &node);

    void close_latest_scope(MutableQuantified &node);

    void eliminate_existential(const IMutableTerm &target_bound_variable);

    static const char * visitor_name;

    std::stack<std::vector<std::unique_ptr<IMutableTerm>>> universally_quantified_variables;

    std::size_t skolem_counter = 0;

    std::unordered_map<std::string_view, std::unique_ptr<IMutableTerm>> skolem_replacements;

    TermResolutionVisitor term_visitor{skolem_replacements};

    std::unique_ptr<IMutableSentence> extracted_sentence;
};

}

#endif
