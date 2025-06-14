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

#include "../../../IR/Mutable/Sentences/IMutableSentenceNode.hpp"
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

    void visit(MutableQuantifiedSentenceNode &node) override;

    void visit(MutablePredicationNode &node) override;

    void visit(MutableIdentitySentenceNode &node) override;

    void visit(MutableConnectedSentenceNode &node) override;

    void visit(MutableSentenceRoot &node) override;

private:
    void open_scope(MutableQuantifiedSentenceNode &node);

    void close_latest_scope(MutableQuantifiedSentenceNode &node);

    void eliminate_existential(const IMutableTermNode &target_bound_variable);

    static const char * visitor_name;

    std::stack<std::vector<std::unique_ptr<IMutableTermNode>>> universally_quantified_variables;

    std::size_t skolem_counter = 0;

    std::unordered_map<std::string, std::unique_ptr<IMutableTermNode>> skolem_replacements;

    TermResolutionVisitor term_visitor{skolem_replacements};

    std::unique_ptr<IMutableSentenceNode> extracted_sentence;
};

}

#endif
