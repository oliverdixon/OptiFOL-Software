/*
 * Copyright (c) All Rights Reserved
 * 2024-2025 Oliver Dixon <od641@york.ac.uk>
 */

//
// Created by owd on 19/11/24.
//

#ifndef OPTIFOL_ITERMNODE_HPP
#define OPTIFOL_ITERMNODE_HPP

#include <string>

namespace optifol
{

class MutatingTermVisitorBase;

class ITermNode
{
public:
    virtual ~ITermNode() = default;

    [[nodiscard]] virtual std::string to_string() const = 0;

    [[nodiscard]] virtual std::string get_disambiguated_name() const = 0;

    virtual void accept(MutatingTermVisitorBase& visitor) = 0;
};

}

#endif //OPTIFOL_ITERMNODE_HPP
