/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */



#ifndef OPTIFOL_CONSTANTNODE_HPP
#define OPTIFOL_CONSTANTNODE_HPP

#include "ITermNode.hpp"

namespace optifol
{

class ConstantNode :
        public ITermNode
{
public:
    [[maybe_unused]] explicit ConstantNode(std::string name) :
            name(std::move(name))
    {}

    [[nodiscard]] std::string to_string() const override
    {
        return name;
    }

    [[nodiscard]] std::string get_disambiguated_name() const override
    {
        return to_string();
    }

    void accept(MutatingTermVisitorBase& visitor) override
    {
        visitor.visit(*this);
    }

private:
    const std::string name;
};

}

#endif
