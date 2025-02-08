/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */



#ifndef OPTIFOL_ISENTENCENODE_HPP
#define OPTIFOL_ISENTENCENODE_HPP

#include <string>

namespace optifol
{

class MutatingSentenceVisitorBase;
class IObservingSentenceVisitor;

class ISentenceNode
{
public:
    virtual ~ISentenceNode() = default;

    virtual void accept(MutatingSentenceVisitorBase &visitor) = 0;

    virtual void accept(IObservingSentenceVisitor &visitor) const = 0;
};

}

#endif //OPTIFOL_ISENTENCENODE_HPP
