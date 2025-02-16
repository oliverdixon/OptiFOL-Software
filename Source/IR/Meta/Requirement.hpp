/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#ifndef REQUIREMENT_HPP
#define REQUIREMENT_HPP

#include <memory>
#include <string>

#include "../../IR/Sentences/ISentenceNode.hpp"

namespace optifol
{

class Requirement
{
public:
    explicit Requirement(std::string&& name, std::unique_ptr<ISentenceNode>&& root_sentence);

    [[nodiscard]] std::string get_name() const;

private:
    const std::string name;

    const std::unique_ptr<ISentenceNode> root_sentence;
};

}

#endif
