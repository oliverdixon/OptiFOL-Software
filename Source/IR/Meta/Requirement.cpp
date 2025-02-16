/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#include "Requirement.hpp"

namespace optifol
{

Requirement::Requirement(std::string &&name, std::unique_ptr<ISentenceNode> &&root_sentence):
    name(std::move(name)), root_sentence(std::move(root_sentence))
{ }

std::string Requirement::get_name() const
{
    return name;
}

}
