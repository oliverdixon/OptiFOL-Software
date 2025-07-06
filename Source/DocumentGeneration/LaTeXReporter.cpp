/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/6/25.
//

#include "LaTeXReporter.hpp"
#include "../Storage/Requirement.hpp"

namespace optifol
{

// TODO for testing only. We should be using resources bundled with executable.
const std::string LaTeXReporter::base_template_path = "Resources/ReportTemplates/LaTeX/report.tex";

LaTeXReporter::LaTeXReporter(const std::string &base_path) :
    requirements_index(base_path + "/index.csv"),
    base_path(base_path)
{
}

void LaTeXReporter::add_requirement(const Requirement &requirement)
{
    requirements_index << requirement.property_name().get_value() << ','
                       << requirement.property_description().get_value() << ','
                       << '$' << requirement.observe_latex_statement() << '$';
}

void LaTeXReporter::generate()
{
    requirements_index.flush();

    popen(("TEXINPUTS=.:" + base_path + ": latexmk -pdf -interaction=nonstopmode -outdir=" + base_path + ' ' +
        base_template_path).c_str(), "r");
}

} // namespace optifol
