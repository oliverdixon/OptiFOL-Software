//
// Created by owd on 19/11/24.
//

#ifndef OPTIFOL_ISENTENCENODE_HPP
#define OPTIFOL_ISENTENCENODE_HPP

#include <string>

namespace optifol
{

class MutatingVisitorBase;
class IObservingVisitor;

class ISentenceNode
{
public:
    virtual ~ISentenceNode() = default;

    virtual void accept(MutatingVisitorBase &visitor) = 0;

    virtual void accept(IObservingVisitor &visitor) const = 0;
};

}

#endif //OPTIFOL_ISENTENCENODE_HPP
