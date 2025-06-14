/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the generic mutable IR Term interface
 * @author Oliver Dixon
 * @date 2025-05-06
 * @version Development
 */

#ifndef IMUTABLETERMNODE_HPP
#define IMUTABLETERMNODE_HPP

#include <memory>
#include <string>
#include <vector>

#include "../../../IHashable.hpp"

namespace optifol
{

class MutableFunctionNode;
class MutableVariableNode;
class UnificationVisitor;
class MutatingTermVisitorBase;

class IMutableTermNode :
        public IHashable
{
public:
    [[nodiscard]] virtual std::unique_ptr<IMutableTermNode> clone() const = 0;

    [[nodiscard]] virtual std::string to_string() const = 0;

    [[nodiscard]] virtual std::string get_disambiguated_name() const = 0;

    virtual void accept(MutatingTermVisitorBase &visitor) = 0;

    [[nodiscard]] std::size_t hash() const noexcept override;

    virtual bool accept(UnificationVisitor &visitor, const MutableVariableNode &target) const;

    virtual bool accept(UnificationVisitor &visitor, const IMutableTermNode &target) const;

    virtual bool accept(UnificationVisitor &visitor, const MutableFunctionNode &target) const;

    bool operator==(const IMutableTermNode &other) const
    {
        return hash() == other.hash();
    }

    virtual std::ostream &serialise(std::ostream &ostream) const = 0;

    friend std::ostream& operator<<(std::ostream& ostream, const IMutableTermNode& object)
    {
        return object.serialise(ostream);
    }

    bool operator==(const std::unique_ptr<IMutableTermNode>& other) const
    {
        return other->hash() == hash();
    }

    bool operator==(const std::shared_ptr<IMutableTermNode>& other) const
    {
        return other->hash() == hash();
    }

private:
    std::vector<std::reference_wrapper<const MutableVariableNode>> substitution_keys;
};

}

#endif
