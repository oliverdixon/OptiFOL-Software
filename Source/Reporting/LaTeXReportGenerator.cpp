/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the LaTeX report-generator backend
 * @author Oliver Dixon
 * @date 2025-08-05
 * @version Development
 */

#include <cassert>
#include <giomm/file.h>
#include <glibmm/miscutils.h>

#include "LaTeXReportGenerator.hpp"
#include "../Storage/Requirement.hpp"
#include "../UserTesting/Modelling/TestGroup.hpp"

namespace optifol
{

LaTeXReportGenerator::LaTeXReportGenerator(const Glib::RefPtr<Gio::File>& output_directory) :
    output_directory(output_directory),
    index_file(Gio::File::create_for_path(this->output_directory->get_path() + "/index.tex")->append_to()),
    tests_file(Gio::File::create_for_path(this->output_directory->get_path() + "/tests.tex")->append_to())
{
    start_requirements();
}

void LaTeXReportGenerator::add_requirement(const Requirement &requirement)
{
    write_property(*index_file, requirement.property_name());
    write_property(*index_file, requirement.property_description());
    write_property(*index_file, std::string(requirement.observe_latex_statement()), true);
    write_property(*index_file, serialise_time(requirement.property_creation_time().get_value()), true);
    write_property(*index_file, serialise_time(requirement.property_modified_time().get_value()), true, true);
}

void LaTeXReportGenerator::add_test_group(const TestGroup &test_group)
{
    tests_file->write("\n\\subsection{" + test_group.property_name().get_value() + '}');
    test_group.for_each([this](const Requirement& requirement)
    {
        tests_file->write("\\subsubsection{" + requirement.property_name().get_value() + "}");
        tests_file->write(R"(
\begin{xltabular}{\linewidth}{lXXl}%
		\toprule\normalfont%
            \textbf{Test Name}&%
            \textbf{Target Executable}&%
            \textbf{Test Fixture}&%
            \textbf{Test Result}%
        \\\midrule\endhead%
		\bottomrule\endfoot%
)");

        const auto tests = requirement.observe_tests();
        const auto test_count = tests->get_n_items();

        for (guint test_index = 0; test_index < test_count; ++test_index) {
            const auto test = tests->get_item(test_index);
            const auto result = test->property_result().get_value();

            write_property(*tests_file, test->property_name());
            write_property(*tests_file, test->property_target_executable_name());
            write_property(*tests_file, test->property_fixture().get_value());

            if (result == nullptr)
                write_property(*tests_file, "", true, true);
            else
                write_property(*tests_file, result->has_passed() ? "Passed" : "Failed", true, true);
        }

        tests_file->write("\\end{xltabular}%\n");
    });
}

void LaTeXReportGenerator::generate(Glib::RefPtr<Gtk::TextBuffer> output, sigc::slot<void()> &&finished_callback)
{
    assert(index_file->is_closed() == false);
    assert(tests_file->is_closed() == false);

    // Close the files.
    end_requirements();
    tests_file->close();

    // Start the sub-process.
    latex_executor.emplace(
        "",
        std::vector<std::string>{
            "latexmk",
            "-pdf",
            "-interaction=nonstopmode",
            "-outdir=" + output_directory->get_path(),
            "Resources/ReportTemplates/LaTeX/report.tex"
        },
        std::vector{
            "PATH=" + Glib::getenv("PATH"),
            "TEXINPUTS=.:" + output_directory->get_path() + ":"
        },
        std::move(output),
        [user_callback = std::move(finished_callback)](const int exit_code)
        {
            /*
             * The interface doesn't care about implementation-specific details such as existence of a sub-process or
             * its system exit code. Throw it away. The sub-process executor will report any faults.
             */
            std::ignore = exit_code;
            user_callback();
        }
    );
}

void LaTeXReportGenerator::start_requirements() const
{
    assert(index_file->is_closed() == false);

    index_file->write(R"(\begin{xltabular}{\linewidth}{lXXll}%
		\toprule\normalfont%
            \textbf{Name}&%
            \textbf{Description}&%
            \textbf{Statement}&%
            \textbf{Created}&%
            \textbf{Modified}%
        \\\midrule\endhead%
		\bottomrule\endfoot%
)");
}

void LaTeXReportGenerator::end_requirements() const
{
    assert(index_file->is_closed() == false);

    index_file->write("\\end{xltabular}%\n");
    index_file->close();

    assert(index_file->is_closed() == true);
}

void LaTeXReportGenerator::write_property(Gio::FileOutputStream &output_stream, const std::string &string,
    const bool verbatim, const bool eol) noexcept
{
    assert(output_stream.is_closed() == false);

    if (string.empty())
        output_stream.write("\\emph{Not provided.}");
    else
        output_stream.write(verbatim ? string : "\\optifolescaped|" + string + '|');

    output_stream.write(eol ? "\\\\\n" : "&");
}

std::string LaTeXReportGenerator::serialise_time(const StorageObjectBase::TimeT &time)
{
    const auto seconds_since_epoch = floor<std::chrono::seconds>(time.time_since_epoch());

    return std::format("\\DTMdisplay{{{:%Y}}}{{{:%m}}}{{{:%d}}}{{-1}}{{{:%H}}}{{{:%M}}}{{{:%S}}}{{1}}{{0}}",
        time, time, time, time, time,
        std::chrono::duration_cast<std::chrono::seconds>(seconds_since_epoch) % std::chrono::minutes(1));
}

} // namespace optifol
