/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the mutable Sentence Root IR node
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#include "MutableSentenceRoot.hpp"

#include "../../../Visitors/MutableTargets/Observers/IObservingNodeVisitor.hpp"
#include "../../../Visitors/MutableTargets/RepositoryBuildingVisitor.hpp"
#include "../../../Visitors/MutableTargets/Sentences/MutatingSentenceVisitorBase.hpp"

namespace optifol
{

MutableSentenceRoot::MutableSentenceRoot(std::unique_ptr<IMutableSentence> &&sentence) :
    sentence(std::move(sentence))
{
}

std::unique_ptr<IMutableSentence> MutableSentenceRoot::clone() const
{
    return std::make_unique<MutableSentenceRoot>(sentence->clone());
}

void MutableSentenceRoot::flip_polarity() noexcept
{
    is_positive = !is_positive;
}

bool MutableSentenceRoot::is_negative_polarity() const noexcept
{
    return !is_positive;
}

std::unique_ptr<IMutableSentence> MutableSentenceRoot::take_sentence() noexcept
{
    return std::move(sentence);
}

const IMutableSentence *MutableSentenceRoot::observe_sentence() const noexcept
{
    return sentence.get();
}

void MutableSentenceRoot::put_sentence(std::unique_ptr<IMutableSentence> &&sentence) noexcept
{
    this->sentence = std::move(sentence);
}

bool MutableSentenceRoot::operator==(const IMutableSentence &other) const noexcept
{
    const auto other_root = dynamic_cast<const MutableSentenceRoot *>(&other);
    if (other_root == nullptr)
        // Other IMutableSentence isn't a MutableSentenceRoot.
        return false;

    if (is_positive != other_root->is_positive)
        // Different signs.
        return false;

    if (sentence == nullptr || other_root->sentence == nullptr)
        return sentence == other_root->sentence;

    return *sentence == *other_root->sentence;
}

void MutableSentenceRoot::accept(MutatingSentenceVisitorBase &visitor)
{
    visitor.visit(*this);
}

void MutableSentenceRoot::accept(IObservingNodeVisitor &visitor) const
{
    visitor.visit(*this);
}

const IProcessedSentence *MutableSentenceRoot::accept(RepositoryBuildingVisitor &visitor)
{
    visitor.visit(*this);
    return nullptr;
}

std::size_t MutableSentenceRoot::hash() const noexcept
{
    return hash_polarity(sentence->hash(), is_negative_polarity());
}

std::ostream &MutableSentenceRoot::serialise(std::ostream &ostream) const
{
    if (is_negative_polarity())
        ostream << '~';

    ostream << '(';
    sentence->serialise(ostream);
    return ostream << ')';
}

} // namespace optifol
