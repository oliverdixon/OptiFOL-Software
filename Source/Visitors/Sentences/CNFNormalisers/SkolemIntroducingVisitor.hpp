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

#include "../MutatingSentenceVisitorBase.hpp"
#include "../../../IR/Sentences/ISentenceNode.hpp"
#include "../../Terms/TermResolutionVisitor.hpp"

namespace optifol
{

class SkolemIntroducingVisitor:
        public MutatingSentenceVisitorBase
{
public:
    SkolemIntroducingVisitor();

    ~SkolemIntroducingVisitor() override;

    std::string_view get_visitor_name() const override;

    void visit(QuantifiedSentenceNode &node) override;

    void visit(PredicationNode &node) override;

    void visit(IdentitySentenceNode &node) override;

    void visit(ConnectedSentenceNode &node) override;

    void visit(SentenceRoot &node) override;

private:
    void open_scope(QuantifiedSentenceNode &node);

    void close_latest_scope(QuantifiedSentenceNode &node);

    void eliminate_existential(const ITermNode &target_bound_variable);

    static const char * visitor_name;

    std::stack<std::vector<std::unique_ptr<ITermNode>>> universally_quantified_variables;

    std::size_t skolem_counter = 0;

    std::unordered_map<std::string, std::unique_ptr<ITermNode>> skolem_replacements;

    TermResolutionVisitor term_visitor{skolem_replacements};

    std::unique_ptr<ISentenceNode> extracted_sentence;
};

}

#endif
