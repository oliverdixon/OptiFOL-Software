/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 7/6/25.
//

#ifndef IDOCUMENTGENERATOR_HPP
#define IDOCUMENTGENERATOR_HPP

namespace optifol
{

class Requirement;

class IDocumentGenerator
{
public:
    virtual ~IDocumentGenerator() = default;

    virtual void add_requirement(const Requirement& requirement) = 0;

    virtual void generate() = 0;
};

} // namespace optifol

#endif // IDOCUMENTGENERATOR_HPP
