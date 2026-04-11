/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the LaTeX report-generator backend
 * @author Oliver Dixon
 * @date 2025-08-05
 * @version Development
 */

#ifndef OPTIFOL_LATEXREPORTGENERATOR_HPP
#define OPTIFOL_LATEXREPORTGENERATOR_HPP

#include <giomm/fileoutputstream.h>

#include "../GUI/StreamingProcessExecutor.hpp"
#include "../Storage/StorageObjectBase.hpp"
#include "IReportGenerator.hpp"

template<typename Candidate>
concept SerialisableProperty = requires(const Glib::PropertyProxy_ReadOnly<Candidate> candidate) {
    { candidate.get_value() } -> std::convertible_to<Glib::ustring>;
};

namespace optifol
{

/**
 * @class LaTeXReportGenerator
 * @brief Provide a LaTeX backend for the IReportGenerator. The implementation takes responsibility for the
 * construction and compilation of the LaTeX document into a PDF using the standard TeX Live
 * <code>latexml</code> utility in a sub-process.
 * @see StreamingProcessExecutor for the <code>latexmk</code> asynchronous invocation mechanism.
 */
class LaTeXReportGenerator : public IReportGenerator
{
public:
    /**
     * @brief Construct a new LaTeXReportGenerator at the given root.
     * @param output_directory The root directory for LaTeX generated and auxiliary files.
     */
    explicit LaTeXReportGenerator(const Glib::RefPtr<Gio::File> &output_directory);

    /**
     * @brief Disable the copy-constructor as objects have locks on external system resources.
     */
    LaTeXReportGenerator(const LaTeXReportGenerator &) = delete;

    void add_requirement(const Requirement &requirement) override;

    void add_test_group(const TestGroup &test_group) override;

    /**
     * @copydoc IReportGenerator::generate
     * @details Asynchronously invokes a <code>latexmk</code> sub-process to compile the TeX-serialised IR
     * nodes into a complete PDF. The results from <code>stdout</code> and <code>stderr</code> of the
     * <code>latexmk</code> process are streamed into the given Gtk::TextBuffer in near-real-time. Upon
     * completion of the sub-process, the given user callback is invoked.
     * @pre The requirements index LaTeX file is open for writing.
     * @pre The tests report LaTeX file is open for writing.
     */
    void generate(Glib::RefPtr<Gtk::TextBuffer> output, sigc::slot<void()> &&finished_callback) override;

private:
    /**
     * @brief Serialise the TeX starting sequence for the requirements index.
     * @pre The requirements stream is open for writing.
     */
    void start_requirements() const;

    /**
     * @brief Serialise the TeX ending sequence for the requirements index.
     * @pre The requirements stream is open for writing.
     */
    void end_requirements() const;

    /**
     * @brief Serialise a single Glib property to the given LaTeX output stream.
     * @tparam PropertyType The property type
     * @param output_stream The destination output stream, open for writing.
     * @param property The string to serialise.
     * @param verbatim Should the string be passed in verbatim to the TeX interpreter, or wrapped in a
     * escaping environment?
     * @param eol Should an end-of-line (EOL) marker be appended to the field?
     */
    template<SerialisableProperty PropertyType>
    static void write_property(Gio::FileOutputStream &output_stream,
            const Glib::PropertyProxy_ReadOnly<PropertyType> property, const bool verbatim = false,
            const bool eol = false)
    {
        write_property(output_stream, property.get_value(), verbatim, eol);
    }

    /**
     * @brief Serialise a string to the given LaTeX output stream.
     * @param output_stream The destination output stream, open for writing.
     * @param string The string to serialise.
     * @param verbatim Should the string be passed in verbatim to the TeX interpreter, or wrapped in a
     * escaping environment?
     * @param eol Should an end-of-line (EOL) marker be appended to the field?
     * @pre The output stream is open for writing.
     */
    static void write_property(Gio::FileOutputStream &output_stream, const std::string &string,
            bool verbatim = false, bool eol = false) noexcept;

    /**
     * @brief Serialises a time point to the LaTeX <code>datetime2</code> format.
     * @details Syntax: <code>\\DTMdisplay{YYYY}{MM}{DD}{-1}{dd}{mm}{ss}{1}{0}</code>
     * @param time The time-point to serialise.
     * @return The LaTeX-serialised <code>datetime2</code> macro.
     */
    static std::string serialise_time(const StorageObjectBase::TimeT &time);

    const Glib::RefPtr<Gio::File> output_directory;

    const Glib::RefPtr<Gio::FileOutputStream> index_file;
    const Glib::RefPtr<Gio::FileOutputStream> tests_file;

    std::optional<StreamingProcessExecutor> latex_executor;
};

} // namespace optifol

#endif // OPTIFOL_LATEXREPORTGENERATOR_HPP
