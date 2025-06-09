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

namespace optifol
{

class FunctionNode;
class VariableNode;
class UnificationVisitor;
class MutatingTermVisitorBase;

class ITermNode :
        public IHashable
{
public:
    [[nodiscard]] virtual std::unique_ptr<ITermNode> clone() const = 0;

    [[nodiscard]] virtual std::string to_string() const = 0;

    [[nodiscard]] virtual std::string get_disambiguated_name() const = 0;

    virtual void accept(MutatingTermVisitorBase &visitor) = 0;

    [[nodiscard]] std::size_t hash() const noexcept override;

    virtual bool accept(UnificationVisitor &visitor, const VariableNode &target) const;

    virtual bool accept(UnificationVisitor &visitor, const ITermNode &target) const;

    virtual bool accept(UnificationVisitor &visitor, const FunctionNode &target) const;

    virtual std::ostream &serialise(std::ostream &ostream) const = 0;

    friend std::ostream& operator<<(std::ostream& ostream, const ITermNode& object)
    {
        return object.serialise(ostream);
    }

private:
    std::vector<std::reference_wrapper<const VariableNode>> substitution_keys;
};

}

#endif
