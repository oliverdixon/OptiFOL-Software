/**
 * @file UniversalEliminationVisitor.hpp
 * @brief Class specification for the Universal-Elimination Visitor and its associated rule set.
 * @author Oliver Dixon
 * @date 2024-11-25
 * @version Development
 */

#ifndef OPTIFOL_UNIVERSALELIMINATIONVISITOR_HPP
#define OPTIFOL_UNIVERSALELIMINATIONVISITOR_HPP

#include "VisitorBase.hpp"

namespace optifol
{

class UniversalEliminationVisitor :
        public VisitorBase
{
public:
    void visit(QuantifiedSentenceNode& node) override;

    void visit(NodeProxy& proxy) override;

    void reset() override;

private:
    std::shared_ptr<ISentenceNode> extracted_sentence;
};

}

#endif //OPTIFOL_UNIVERSALELIMINATIONVISITOR_HPP
