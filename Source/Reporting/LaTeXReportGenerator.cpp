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

#include "LaTeXReportGenerator.hpp"

#include <assert.h>
#include <giomm/file.h>
#include <giomm/resource.h>
#include <glibmm/miscutils.h>

#include "../Storage/Requirement.hpp"
#include "../UserTesting/Modelling/TestGroup.hpp"

namespace optifol
{

LaTeXReportGenerator::LaTeXReportGenerator(const Glib::RefPtr<Gio::File> output_directory) :
    output_directory(std::move(output_directory)),
    index_file(Gio::File::create_for_path(this->output_directory->get_path() + "/index.tex")->append_to()),
    tests_file(Gio::File::create_for_path(this->output_directory->get_path() + "/tests.tex")->append_to())
{
    start_requirements();
}

void LaTeXReportGenerator::add_requirement(const Requirement &requirement)
{
    index_file->write(requirement.property_name().get_value() + '&');
    index_file->write(requirement.property_description().get_value() + '&');

    const auto latex_statement = requirement.observe_latex_statement();
    if (latex_statement.empty())
        index_file->write("\\emph{Not provided.}");
    else
        index_file->write(std::string(latex_statement));

    index_file->write('&' + serialise_time(requirement.property_creation_time().get_value()));
    index_file->write('&' + serialise_time(requirement.property_modified_time().get_value()));

    index_file->write("\\\\");
}

void LaTeXReportGenerator::add_test_group(const TestGroup &test_group)
{
    tests_file->write("\\subsection{" + test_group.property_name().get_value() + '}');
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

        const auto tests = requirement.get_tests();
        const auto test_count = tests->get_n_items();

        for (guint test_index = 0; test_index < test_count; ++test_index) {
            const auto test = tests->get_item(test_index);
            const auto result = test->property_result().get_value();

            tests_file->write("\\optifolescaped|" + test->property_name().get_value() + "|&");
            tests_file->write("\\optifolescaped|" + test->property_target_executable_name().get_value() + "|&");
            tests_file->write("\\optifolescaped|" + test->property_fixture().get_value() + "|&");

            if (result == nullptr)
                tests_file->write("Unknown");
            else
                tests_file->write(result->has_passed() ? "Passed" : "Failed");

            tests_file->write("\\\\");
        }

        tests_file->write(R"(\end{xltabular}%)");
    });
}

void LaTeXReportGenerator::generate(const Glib::RefPtr<Gtk::TextBuffer> &output, sigc::slot<void()> &&finished_callback)
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

    index_file->write(R"(
\begin{xltabular}{\linewidth}{>{\ttfamily}lXXll}%
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

    index_file->write(R"(\end{xltabular}%)");
    index_file->close();

    assert(index_file->is_closed() == true);
}

std::string LaTeXReportGenerator::serialise_time(const StorageObjectBase::TimeT &time)
{
    const auto seconds_since_epoch = floor<std::chrono::seconds>(time.time_since_epoch());

    return std::format("\\DTMdisplay{{{:%Y}}}{{{:%m}}}{{{:%d}}}{{-1}}{{{:%H}}}{{{:%M}}}{{{:%S}}}{{1}}{{0}}",
        time, time, time, time, time,
        std::chrono::duration_cast<std::chrono::seconds>(seconds_since_epoch) % std::chrono::minutes(1));
}

} // namespace optifol
