/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 8/2/25.
//

#ifndef GOOGLEEXECUTIONGROUP_HPP
#define GOOGLEEXECUTIONGROUP_HPP

#include <unordered_map>

#include "../../GUI/ProcessExecutor.hpp"
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

    void remove_test(Glib::RefPtr<Test> target_test) override;

private:
    void distribute_results(int exit_code) const;

    static constexpr char pattern_separator = ':';
    static constexpr char component_separator = '.';

    static constexpr guint16 minimum_port_number = 1024; // First unprivileged port number under Linux.
    static constexpr guint16 maximum_port_number = std::numeric_limits<guint16>::max();
    static guint16 port_number;

    std::string filter_line;
    std::unique_ptr<ProcessExecutor> executor;
    std::unique_ptr<TestListenerBase> listener;

    using FilterLineIterator = decltype(filter_line)::const_iterator;
    std::unordered_map<Glib::RefPtr<Test>, std::pair<FilterLineIterator, FilterLineIterator>> tests;
};

} // namespace optifol

#endif // GOOGLEEXECUTIONGROUP_HPP
