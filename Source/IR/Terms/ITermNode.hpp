/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the generic IR Term interface
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef ITERMNODE_HPP
#define ITERMNODE_HPP

#include <memory>
#include <string>
#include <vector>

#include "../../IHashable.hpp"
#include "../Support/UnifyCandidateBase.hpp"

namespace optifol
{

class MutatingTermVisitorBase;

class ITermNode :
        public IHashable,
        public UnifyCandidateBase
{
public:
    ~ITermNode() override;

    [[nodiscard]] virtual std::unique_ptr<ITermNode> clone() const = 0;

    [[nodiscard]] virtual std::string to_string() const = 0;

    [[nodiscard]] virtual std::string get_disambiguated_name() const = 0;

    virtual void accept(MutatingTermVisitorBase &visitor) = 0;

    [[nodiscard]] std::size_t hash() const noexcept override
    {
        return std::hash<std::string>{}(get_disambiguated_name());
    }

    bool unify_with_me(const VariableNode &variable) override;

protected:
    void register_substitution(const VariableNode &bound_key, ITermNode &bound_value);

private:
    std::vector<std::reference_wrapper<const VariableNode>> substitution_keys;
};

}

#endif
