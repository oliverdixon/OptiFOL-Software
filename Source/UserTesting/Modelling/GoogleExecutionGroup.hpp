/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the Google Test execution group
 * @author Oliver Dixon
 * @date 2025-08-08
 * @version Development
 */

#ifndef GOOGLEEXECUTIONGROUP_HPP
#define GOOGLEEXECUTIONGROUP_HPP

#include <forward_list>

#include "../../GUI/ProcessExecutor.hpp"
#include "Test.hpp"
#include "ExecutionGroup.hpp"
#include "TestListenerBase.hpp"

namespace optifol
{

class GoogleExecutionGroup : public ExecutionGroup
{
public:
    explicit GoogleExecutionGroup(Glib::RefPtr<Test> initial_test);

    void run() override;

    void add_test(Glib::RefPtr<Test> new_test) override;

    void remove_test(const Glib::RefPtr<Test> &target_test) override;

    /**
     * @brief Queries the empty state of the Test list.
     * @return Does the ExecutionGroup contain any Test objects?
     */
    [[nodiscard]] std::size_t is_empty() const noexcept override;

private:
    /**
     * @brief Handle the sub-process exit by distributing results from the listener pool to the Requirement objects.
     * @param exit_code Exit code from the Google Test sub-process.
     */
    void distribute_results(int exit_code) const;

    /**
     * @brief Force a rebuild of the @ref filter_line_cache string from the @ref tests content.
     */
    void invalidate_cache();

    static constexpr char pattern_separator = ':';
    static constexpr char component_separator = '.';

    static constexpr guint16 minimum_port_number = 1024; // First unprivileged port number under Linux.
    static constexpr guint16 maximum_port_number = std::numeric_limits<guint16>::max();
    static guint16 port_number;

    std::unique_ptr<ProcessExecutor> executor;
    std::unique_ptr<TestListenerBase> listener;

    std::forward_list<Glib::RefPtr<Test>> tests;

    /**
     * @brief The Google Test filter specification for the loaded @ref tests. The Google Test format for a single
     *  fixture-test pair is <code>fixture.test:</code>. The colon is a delimiter and may be trailing.
     */
    std::string filter_line_cache;

    /**
     * @brief Is @ref filter_line_cache a representative Google Test filter string for the detained @ref tests?
     */
    bool cache_ok = true;
};

} // namespace optifol

#endif // GOOGLEEXECUTIONGROUP_HPP
