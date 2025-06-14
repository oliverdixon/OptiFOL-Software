/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Quantifier Extracting Visitor
 * @author Oliver Dixon
 * @date 2025-05-04
 * @version Development
 */

#ifndef QUANTIFIEREXTRACTINGVISITOR_HPP
#define QUANTIFIEREXTRACTINGVISITOR_HPP

#include <memory>
#include <optional>

#include "../../../IR/Mutable/Sentences/IMutableSentenceNode.hpp"
#include "../../../IR/Mutable/Sentences/MutableQuantifiedSentenceNode.hpp"
#include "../../../IR/Mutable/Terms/IMutableTermNode.hpp"
#include "../MutatingSentenceVisitorBase.hpp"

namespace optifol
{

class QuantifierExtractingVisitor:
        public MutatingSentenceVisitorBase
{
public:
    [[nodiscard]] std::string_view get_visitor_name() const override;

    void visit(MutableConnectedSentenceNode &node) override;

    void visit(MutableQuantifiedSentenceNode &node) override;

    void visit(MutableSentenceRoot &node) override;

private:
    struct QuantifiedTemplate
    {
        QuantifiedTemplate(const QuantifierTypes type, std::unique_ptr<IMutableTermNode>&& bound_term,
                std::unique_ptr<IMutableSentenceNode>&& sentence, MutableQuantifiedSentenceNode * owner) :
            type(type),
            bound_term(std::move(bound_term)),
            sentence(std::move(sentence)),
            owner(owner)
        { }

        void return_to_owner()
        {
            owner->put_sentence(std::move(sentence));
            owner->put_bound_term(std::move(bound_term));
        }

        QuantifierTypes type;
        std::unique_ptr<IMutableTermNode> bound_term;
        std::unique_ptr<IMutableSentenceNode> sentence;

    private:
        MutableQuantifiedSentenceNode * owner;
    };

    enum class TrackingMode
    {
        NotTracking,
        LeftMajor,
        RightMajor
    };

    static const char * visitor_name;

    TrackingMode tracking_mode = TrackingMode::NotTracking;
    std::optional<QuantifiedTemplate> quant_lhs_data;
    std::optional<QuantifiedTemplate> quant_rhs_data;
    std::optional<std::pair<QuantifierTypes, std::unique_ptr<IMutableTermNode>>> transformation_metadata;
};

}

#endif
