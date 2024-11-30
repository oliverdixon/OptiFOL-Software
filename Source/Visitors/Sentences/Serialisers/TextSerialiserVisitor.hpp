//
// Created by owd on 29/11/24.
//

#ifndef OPTIFOL_TEXTSERIALISERVISITOR_HPP
#define OPTIFOL_TEXTSERIALISERVISITOR_HPP

#include <sstream>

#include "../IObservingVisitor.hpp"

namespace optifol
{

class TextSerialiserVisitor :
        public IObservingVisitor
{
public:
    using OutT = std::string;

    void visit(const QuantifiedSentenceNode& node) override;

    void visit(const ConnectedSentenceNode& node) override;

    void visit(const NegatedSentenceNode& node) override;

    void visit(const NodeProxy& node) override;

    void visit(const IdentitySentenceNode& node) override;

    void visit(const PredicationNode& node) override;

    std::any extract() const override;

    void reset() override;

private:
    std::ostringstream output_stream;

    [[nodiscard]] static const char *get_operator_symbol(BinaryOperatorTypes type);

    [[nodiscard]] static const char *get_operator_symbol(QuantifierTypes type);
};

}


#endif //OPTIFOL_TEXTSERIALISERVISITOR_HPP
