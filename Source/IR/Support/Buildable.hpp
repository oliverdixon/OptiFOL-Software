/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class template specification for buildable Optifol objects
 * @author Oliver Dixon
 * @date 2025-06-08
 * @version Development
 */

#ifndef BUILDABLE_HPP
#define BUILDABLE_HPP

#include <memory>

namespace optifol
{

/**
 * @class Buildable
 * @brief The Buildable class provides a CRTP-templated base to classes wishing to be trivially wrapped into a C++
 *  unique pointer.
 * @tparam BuildType The concrete class type to build
 */
template<typename BuildType>
class Buildable
{
public:
    /**
     * @brief Construct a class with the given constructor arguments in a unique pointer
     * @tparam PointerType The stored pointer type of the constructed unique pointer; must be a base of, or equal to,
     *  the class BuildType template parameter.
     * @tparam CtorArgs The template parameter pack of argument types to forward to the BuildType constructor.
     * @param args The argument values, corresponding to the given types, to perfectly forward to the constructor
     * @return An instance of the BuildType class, constructed according to the given arguments, wrapped in a unique
     *  pointer containing a pointer to the specified type.
     */
    template<typename PointerType = BuildType, typename... CtorArgs> requires std::derived_from<BuildType, PointerType>
    static std::unique_ptr<PointerType> build(CtorArgs&&... args)
    {
        return std::make_unique<BuildType>(std::forward<CtorArgs>(args)...);
    }
};

}

#endif
