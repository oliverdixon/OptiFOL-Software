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

#include <memory>

namespace optifol
{

class TestResult;

/**
 * @class TestListenerBase
 * @brief Provides a test-framework-agnostic listener to accept network-streamed payloads describing results of
 *  automated tests. Asynchronous network operations are provided by the Glib socket abstraction layers.
 */
class TestListenerBase
{
public:
    /**
     * @brief Register the callback to inform clients of newly parsed TestResult objects
     * @param result_callback The callback capable of taking exclusive ownership of the TestResult object from the
     *  payload protocol parser
     */
    explicit TestListenerBase(sigc::slot<void(std::unique_ptr<TestResult> &&)>&& result_callback) :
        result_callback(std::move(result_callback))
    {
    }

    /**
     * @brief De-registers the callback bindings.
     */
    virtual ~TestListenerBase()
    {
        result_callback.disconnect();
    }

    /**
     * @brief Reports a newly formed TestResult object to the callback
     * @param test_result The owning container of the constructed TestResult
     */
    void report_result(std::unique_ptr<TestResult>&& test_result) const
    {
        result_callback(std::move(test_result));
    }

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
    sigc::slot<void(std::unique_ptr<TestResult>&&)> result_callback;
};

} // namespace optifol

#endif // TESTLISTENERBASE_HPP
