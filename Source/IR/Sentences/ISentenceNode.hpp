/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */



#ifndef OPTIFOL_ISENTENCENODE_HPP
#define OPTIFOL_ISENTENCENODE_HPP
#include <memory>

namespace optifol
{

class MutatingSentenceVisitorBase;
class IObservingSentenceVisitor;

class ISentenceNode
{
public:
    virtual ~ISentenceNode() = default;

    virtual void flip_polarity() = 0;

    [[nodiscard]] virtual bool is_negative_polarity() const = 0;

    [[nodiscard]] virtual std::unique_ptr<ISentenceNode> clone() const = 0;

    virtual void accept(MutatingSentenceVisitorBase &visitor) = 0;

    virtual void accept(IObservingSentenceVisitor &visitor) const = 0;
};

}

#endif
