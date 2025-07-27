/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the generic automated test socket listener
 * @author Oliver Dixon
 * @date 2025-07-20
 * @version Development
 */

#ifndef TESTLISTENERBASE_HPP
#define TESTLISTENERBASE_HPP

#include <giomm/asyncresult.h>
#include <glibmm/refptr.h>
#include <log4cxx/logger.h>
#include <memory>
#include <unordered_set>

#include "../DereferencingEqualityFunctor.hpp"
#include "TestResult.hpp"

namespace optifol
{

class Requirement;

/**
 * @class TestListenerBase
 * @brief Provides a test-framework-agnostic listener to accept network-streamed payloads describing results of
 *  automated tests. Asynchronous network operations are provided by the Glib socket abstraction layers. Received
 *  TestResult objects are stored by the listener in a blob and can be distributed (through shared ownership) to
 *  relevant Requirement objects with @ref endow_requirement.
 */
class TestListenerBase
{
public:
    /**
     * @brief Destruct the TestListenerBase, discarding any unused TestResult objects and closing any opened network
     *  state.
     */
    virtual ~TestListenerBase() = default;

    /**
     * @brief Accepts a newly formed TestResult object
     * @param test_result The owning container of the constructed TestResult
     */
    void accept_result(std::unique_ptr<TestResult> &&test_result);

    /**
     * @brief Given a Requirement with an associated Test object, determine whether a stored TestResult matches the test
     *  specification of the Requirement. If it does, share the TestResult with the Requirement.
     * @param requirement The Requirement to consider sharing the TestResult
     * @throws SemanticException if the Requirement refused the TestResult
     * @see Requirement::emplace_test_result
     */
    void endow_requirement(Requirement &requirement);

protected:
    /**
     * @brief Handle an Glib-asynchronously accepted connection from a client, ready to receive data.
     * @param result The result of the asynchronous socket operation.
     * @note All implementations of this member function must be noexcept as they are called from an asynchronous
     *  Glib-defined context. Any exceptions thrown by composed routines should be absorbed by the callback and logged
     *  using log4cxx instance for the implementation class.
     */
    virtual void connection_callback(const Glib::RefPtr<Gio::AsyncResult> &result) noexcept = 0;

private:
    std::unordered_set<std::shared_ptr<TestResult>, std::hash<TestResult>,
        DereferencingEqualityFunctor<std::shared_ptr<TestResult>, TestResult>> received_test_blob;
};

} // namespace optifol

#endif // TESTLISTENERBASE_HPP
