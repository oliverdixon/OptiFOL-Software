/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the priority queue of Resolvents
 * @author Oliver Dixon
 * @date 2026-01-29
 * @version Development
 */

#include "ResolventQueue.hpp"

#include <algorithm>
#include <cassert>

#include "Resolvent.hpp"

namespace optifol
{

void ResolventQueue::push(Resolvent element, std::unique_ptr<Clause> &&resolution)
{
    assert(element.observe_substance() == resolution.get());

    auto [resolution_it, was_new] = resolutions.insert(std::move(resolution));
    if (!was_new) {
        /*
         * If the insertion could not occur, one of two things has happened:
         *
         *  1. The insertion genuinely failed, indicating a runtime error; or
         *
         *  2. The resolution clause of the incoming resolvent is already owned by us. Therefore, we update the pointer
         *     (using the "friend" access rights conferred by Resolvent) such that it observes a consistently owned
         *     clause. Once this process is repeated for all resolvents, there will be a de-duplicated set of resolution
         *     clauses (either owned by us or transferred into a QueryResult) to which all resolvents correctly refer.
         */
        resolution_it = resolutions.find(*element.observe_substance());
        if (resolution_it == resolutions.end())
            throw std::runtime_error("The ownership of the incoming resolution clause could not be transferred into "
                                     "the queue.");
        element.resolution = resolution_it->get();
    }

    elements.push_back(std::move(element));
    std::ranges::push_heap(elements, comparator); // NOLINT(*-use-transparent-functors)
}

Resolvent ResolventQueue::pop()
{
    if (empty())
        throw std::runtime_error("The ResolventQueue is empty.");

    std::ranges::pop_heap(elements, comparator); // NOLINT(*-use-transparent-functors)
    auto resolvent = std::move(elements.back());
    elements.pop_back();
    return resolvent;
}

std::unique_ptr<Clause> ResolventQueue::extract_resolution(const Resolvent &resolvent)
{
    const auto resolution_it = resolutions.find(*resolvent.observe_substance());
    if (resolution_it == resolutions.end())
        return {};

    return std::move(resolutions.extract(resolution_it).value());
}

bool ResolventQueue::empty() const noexcept
{
    return elements.empty();
}

std::size_t ResolventQueue::size() const noexcept
{
    return elements.size();
}

void ResolventQueue::dump(std::deque<Resolvent> &resolvent_destination, UniqueUnorderedSet<Clause> &clause_destination)
{
    while (!empty()) {
        auto resolvent = pop();
        auto resolution = extract_resolution(resolvent);
        resolvent_destination.push_back(std::move(resolvent));

        if (resolution != nullptr)
            clause_destination.insert(std::move(resolution));
    }
}

} // namespace optifol
