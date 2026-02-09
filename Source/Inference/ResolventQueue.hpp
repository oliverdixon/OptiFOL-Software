/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the priority queue of Resolvents
 * @author Oliver Dixon
 * @date 2026-01-29
 * @version Development
 */

#ifndef OPTIFOL_RESOLVENTQUEUE
#define OPTIFOL_RESOLVENTQUEUE

#include <deque>
#include <memory>

#include "../IR/Sentences/Clause.hpp"

namespace optifol
{
class FVIKnowledgeBase;
class Resolvent;

/**
 * @class ResolventQueue
 * @brief The ResolventQueue is a specialised @ref std::priority_queue style of max-heap enabling ordered retrieval of
 *  Resolvents and temporary unstructured ownership of their corresponding resolution clauses.
 */
class ResolventQueue
{
public:
    /**
     * @brief Push a new Resolvent to the queue.
     * @param element The Resolvent value to push.
     * @param resolution An owning container for the resolution Clause i.a.w. the given Resolvent.
     * @pre The resolution owned by the given Resolvent must refer to the resolution Clause in the given
     *  @ref std::unique_ptr.
     */
    void push(Resolvent element, std::unique_ptr<Clause> &&resolution);

    /**
     * @brief Pop and return the maximal Resolvent from the queue, where order is determined by the @ref std::less
     *  functor on Resolvent. This does not change ownership of resolution Clauses.
     * @return The maximal Resolvent owned by the queue.
     * @throws std::runtime_error if the resolvent queue is empty.
     */
    Resolvent pop();

    /**
     * @brief Extract and transfer ownership of the resolution Clause referred to by the given Resolvent.
     * @param resolvent The Resolvent which resolves to the desired Clause.
     * @return An owning container for the resolution Clause.
     */
    std::unique_ptr<Clause> extract_resolution(const Resolvent& resolvent);

    /**
     * @brief Confer ownership of a resolution Clause into the ResolventQueue.
     * @param resolution The resolution to store.
     */
    bool store_resolution(std::unique_ptr<Clause> &&resolution);

    /**
     * @brief Checks emptiness state of the queue
     * @return Is the queue empty?
     */
    [[nodiscard]] bool empty() const noexcept;

    /**
     * @brief Checks size of the queue
     * @return How many Resolvents are on the queue?
     */
    [[nodiscard]] std::size_t size() const noexcept;

    void dump(std::deque<Resolvent> &resolvent_destination, FVIKnowledgeBase &clause_destination);

private:
    std::deque<Resolvent> elements;
    std::less<Resolvent> comparator{};
    UniqueUnorderedSet<Clause> resolutions;
};

} // namespace optifol

#endif // OPTIFOL_RESOLVENTQUEUE
