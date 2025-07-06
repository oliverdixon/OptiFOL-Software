/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/6/25.
//

#ifndef LATEXREPORTER_HPP
#define LATEXREPORTER_HPP

#include <fstream>

#include "IDocumentGenerator.hpp"

namespace optifol
{

class LaTeXReporter :
        public IDocumentGenerator
{
public:
    explicit LaTeXReporter(const std::string& base_path);

    void add_requirement(const Requirement &requirement) override;

    void generate() override;

private:
    std::ofstream requirements_index;

    const std::string base_path;

    static const std::string base_template_path;
};

} // namespace optifol

#endif // LATEXREPORTER_HPP
