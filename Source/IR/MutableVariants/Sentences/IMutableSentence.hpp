/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the generic mutable IR Sentence interface
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef IMUTABLESENTENCE_HPP
#define IMUTABLESENTENCE_HPP

#include <memory>

#include "../../Sentences/ISentence.hpp"

namespace optifol
{

class MutatingSentenceVisitorBase;
class IObservingSentenceVisitor;

class IMutableSentence :
        public ISentence
{
public:
    [[nodiscard]] virtual std::unique_ptr<IMutableSentence> clone() const = 0;

    virtual void accept(MutatingSentenceVisitorBase &visitor) = 0;

    virtual void accept(IObservingSentenceVisitor &visitor) const = 0;
};

}

#endif
