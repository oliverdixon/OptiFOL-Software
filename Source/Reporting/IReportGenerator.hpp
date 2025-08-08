/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the report generator interface
 * @author Oliver Dixon
 * @date 2025-08-05
 * @version Development
 */

#ifndef OPTIFOL_IREPORTGENERATOR_HPP
#define OPTIFOL_IREPORTGENERATOR_HPP

namespace optifol
{

class Requirement;
class TestGroup;

/**
 * @class IReportGenerator
 * @brief The report generator interface provides a set of operations for generating reports based on subsystems,
 *  enumerating their constituent Requirements index, analysis results, and automated test evidence.
 */
class IReportGenerator
{
public:
    /**
     * @brief Destruct the report generator instance, flushing and releasing any open system resources.
     */
    virtual ~IReportGenerator() = default;

    /**
     * @brief Add a Requirement to the requirements index.
     * @param requirement The Requirement to serialise into the report index.
     */
    virtual void add_requirement(const Requirement &requirement) = 0;

    /**
     * @brief Add a TestGroup, and all containing Requirement and Test objects, to the automated tests section.
     * @param test_group The TestGroup to serialise.
     */
    virtual void add_test_group(const TestGroup &test_group) = 0;

    /**
     * @brief Asynchronously finalise the generation of the report, streaming output to the given buffer.
     * @param output The Gtk::TextBuffer to which compilation output should be streamed.
     * @param finished_callback The callback to execute upon completion of the asynchronous generation.
     */
    virtual void generate(Glib::RefPtr<Gtk::TextBuffer> output, sigc::slot<void()> &&finished_callback) = 0;
};

} // namespace optifol

#endif // OPTIFOL_IREPORTGENERATOR_HPP
