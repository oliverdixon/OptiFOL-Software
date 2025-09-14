/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Optifol namespace declaration required for documentary purposes
 * @author Oliver Dixon
 * @date 2025-03-03
 * @version Development
 */

#ifndef OPTIFOL_HPP
#define OPTIFOL_HPP

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmacro-redefined" // Redefinition is intentional.
#define RAPIDJSON_PARSE_ERROR_NORETURN(parse_error_code, offset) \
    throw optifol::ParseError(#parse_error_code)
#pragma clang diagnostic pop

#include <memory>
#include <unordered_map>
#include <unordered_set>

/**
 * @namespace optifol
 * @brief The Optifol namespace contains the vast majority of the symbols defined by the Optifol client application
 * @note In a proportionally small number of cases, it is necessary to define symbols in the namespaces of the standard
 *  library or various third-party libraries. No such usage is UB, and all such symbol definitions are the result of
 *  template specialisation.
 */
namespace optifol
{

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
/**
 * @concept WeaklyEqualityComparableWith
 * @brief Modelled if and only if the given types may be tested for equality and inequality using
 *  <code>operator==</code> and <code>operator!=</code> member functions, without requiring a common type.
 * @tparam T The first type
 * @tparam U The second type
 * @details This concept mirrors the standard @ref std::equality_comparable_with, without the requirement to model
 *  @ref std::common_reference_with, hence allowing comparisons without converting constructors.
 */
template<class T, class U>
concept WeaklyEqualityComparableWith =
    std::equality_comparable<T> &&
    std::equality_comparable<U> &&
    requires(const std::remove_reference_t<T>& t, const std::remove_reference_t<U>& u)
{
    { t == u } -> std::convertible_to<bool>;
    { t != u } -> std::convertible_to<bool>;
    { u == t } -> std::convertible_to<bool>;
    { u != t } -> std::convertible_to<bool>;
};
#pragma clang diagnostic pop

/**
 * @class UniversalTransparentEquality
 * @brief Provides a transparent equality functor for homogenously comparable types through @ref std::unique_ptr,
 *  @ref std::shared_ptr, and raw pointers; or heterogeneously comparing through object references where
 *  WeaklyEqualityComparableWith is modelled.
 * @tparam T The concrete type to transparently compare.
 */
template<typename T>
struct UniversalTransparentEquality
{
    using is_transparent = void;

    // Object reference on LHS

    bool operator()(const T& obj, const T& obj_other) const noexcept
    {
        return obj == obj_other;
    }

    bool operator()(const T& obj, const std::unique_ptr<T>& ptr) const noexcept
    {
        if (ptr == nullptr)
            return false;

        return *ptr == obj;
    }

    bool operator()(const T& obj, const std::shared_ptr<T>& ptr) const noexcept
    {
        if (ptr == nullptr)
            return false;

        return *ptr == obj;
    }

    bool operator()(const T& obj, const T * const ptr) const noexcept
    {
        if (ptr == nullptr)
            return false;

        return *ptr == obj;
    }

    template<typename U> requires WeaklyEqualityComparableWith<T, U>
    bool operator()(const T& obj, U&& other) const noexcept
    {
        return obj == other;
    }

    // Unique pointer on LHS

    bool operator()(const std::unique_ptr<T>& ptr, const std::unique_ptr<T>& ptr_other) const noexcept
    {
        if (ptr == nullptr || ptr_other == nullptr)
            return ptr == ptr_other;

        return *ptr == *ptr_other;
    }

    bool operator()(const std::unique_ptr<T>& ptr, const T& obj) const noexcept
    {
        if (ptr == nullptr)
            return false;

        return *ptr == obj;
    }

    bool operator()(const std::unique_ptr<T>& ptr, const std::shared_ptr<T>& ptr_other) const noexcept
    {
        if (ptr == nullptr || ptr_other == nullptr)
            return ptr == ptr_other;

        return *ptr == *ptr_other;
    }

    bool operator()(const std::unique_ptr<T>& ptr, const T * const ptr_other) const noexcept
    {
        if (ptr == nullptr || ptr_other == nullptr)
            return ptr == ptr_other;

        return *ptr == *ptr_other;
    }

    template<typename U> requires WeaklyEqualityComparableWith<T, U>
    bool operator()(const std::unique_ptr<T>& ptr, U&& other) const noexcept
    {
        if (ptr == nullptr)
            return false;

        return *ptr == other;
    }

    // Shared pointer on LHS

    bool operator()(const std::shared_ptr<T>& ptr, const std::shared_ptr<T>& ptr_other) const noexcept
    {
        if (ptr == nullptr || ptr_other == nullptr)
            return ptr == ptr_other;

        return *ptr == *ptr_other;
    }

    bool operator()(const std::shared_ptr<T>& ptr, const T& obj) const noexcept
    {
        if (ptr == nullptr)
            return false;

        return *ptr == obj;
    }

    bool operator()(const std::shared_ptr<T>& ptr, const std::unique_ptr<T>& ptr_other) const noexcept
    {
        if (ptr == nullptr || ptr_other == nullptr)
            return ptr == ptr_other;

        return *ptr == *ptr_other;
    }

    bool operator()(const std::shared_ptr<T>& ptr, const T * const ptr_other) const noexcept
    {
        if (ptr == nullptr || ptr_other == nullptr)
            return ptr == ptr_other;

        return *ptr == *ptr_other;
    }

    template<typename U> requires WeaklyEqualityComparableWith<T, U>
    bool operator()(const std::shared_ptr<T>& ptr, U&& other) const noexcept
    {
        if (ptr == nullptr)
            return false;

        return *ptr == other;
    }

    // Raw pointer on LHS

    bool operator()(const T * const ptr, const T * const ptr_other) const noexcept
    {
        if (ptr == nullptr || ptr_other == nullptr)
            return ptr == ptr_other;

        return *ptr == *ptr_other;
    }

    bool operator()(const T * const ptr, const T& obj) const noexcept
    {
        if (ptr == nullptr)
            return false;

        return *ptr == obj;
    }

    bool operator()(const T * const ptr, const std::unique_ptr<T>& ptr_other) const noexcept
    {
        if (ptr == nullptr || ptr_other == nullptr)
            return ptr == ptr_other;

        return *ptr == *ptr_other;
    }

    bool operator()(const T * const ptr, const std::shared_ptr<T>& ptr_other) const noexcept
    {
        if (ptr == nullptr || ptr_other == nullptr)
            return ptr == ptr_other;

        return *ptr == *ptr_other;
    }

    template<typename U> requires WeaklyEqualityComparableWith<T, U>
    bool operator()(const T * const ptr, U&& other) const noexcept
    {
        if (ptr == nullptr)
            return false;

        return *ptr == other;
    }

    // Dynamically typed object on LHS

    template<typename U> requires WeaklyEqualityComparableWith<T, U>
    bool operator()(U&& obj, const T& obj_other) const noexcept
    {
        return obj == obj_other;
    }

    template<typename U> requires WeaklyEqualityComparableWith<T, U>
    bool operator()(U&& obj, const std::unique_ptr<T>& ptr_other) const noexcept
    {
        if (ptr_other == nullptr)
            return false;

        return obj == *ptr_other;
    }

    template<typename U> requires WeaklyEqualityComparableWith<T, U>
    bool operator()(U&& obj, const std::shared_ptr<T>& ptr_other) const noexcept
    {
        if (ptr_other == nullptr)
            return false;

        return obj == *ptr_other;
    }

    template<typename U> requires WeaklyEqualityComparableWith<T, U>
    bool operator()(U&& obj, const T * const ptr_other) const noexcept
    {
        if (ptr_other == nullptr)
            return false;

        return obj == *ptr_other;
    }

    template<typename U> requires WeaklyEqualityComparableWith<T, U>
    bool operator()(U&& obj, U&& other_obj) const noexcept = delete;
};


/**
 * @typedef UniqueUnorderedSet
 * @brief The @ref std::unordered_set collection to store the templated type <code>T</code> within a transparently
 *  hashable and comparable @ref std::unique_ptr.
 * @tparam T The type to store, detained within @ref std::unique_ptr.
 */
template<typename T>
using UniqueUnorderedSet = std::unordered_set<std::unique_ptr<T>, std::hash<T>, UniversalTransparentEquality<T>>;

/**
 * @typedef UniqueUnorderedMap
 * @brief The @ref std::unordered_map collection to store the templated type <code>T</code> within a transparently
 *  hashable and comparable @ref std::unique_ptr.
 * @tparam K The key type to store, detained within @ref std::unique_ptr.
 * @tparam V The value type to associate with keys.
 */
template<typename K, typename V>
using UniqueUnorderedMap = std::unordered_map<std::unique_ptr<K>, V, std::hash<K>, UniversalTransparentEquality<K>>;

/**
 * @typedef SharedUnorderedSet
 * @brief The @ref std::unordered_set collection to store the templated type <code>T</code> within a transparently
 *  hashable and comparable @ref std::shared_ptr.
 * @tparam T The type to store, detained within @ref std::shared_ptr.
 */
template<typename T>
using SharedUnorderedSet = std::unordered_set<std::shared_ptr<T>, std::hash<T>, UniversalTransparentEquality<T>>;

/**
 * @typedef SharedUnorderedMap
 * @brief The @ref std::unordered_map collection to store the templated type <code>T</code> within a transparently
 *  hashable and comparable @ref std::shared_ptr.
 * @tparam K The key type to store, detained within @ref std::shared_ptr.
 * @tparam V The value type to associate with keys.
 */
template<typename K, typename V>
using SharedUnorderedMap = std::unordered_map<std::shared_ptr<K>, V, std::hash<K>, UniversalTransparentEquality<K>>;

/**
 * @typedef RawUnorderedSet
 * @brief The @ref std::unordered_set collection to store the templated type <code>T</code> within a transparently
 *  hashable and comparable <code>T*</code>.
 * @tparam T The type to store, referenced through a raw pointer.
 */
template<typename T>
using RawUnorderedSet = std::unordered_set<std::add_pointer_t<T>, std::hash<T>, UniversalTransparentEquality<T>>;

/**
 * @typedef RawUnorderedMap
 * @brief The @ref std::unordered_map collection to store the templated type <code>T</code> within a transparently
 *  hashable and comparable <code>T*</code>.
 * @tparam K The key type to store, referenced through a raw pointer.
 * @tparam V The value type to associate with keys.
 */
template<typename K, typename V>
using RawUnorderedMap = std::unordered_map<std::add_pointer_t<K>, V, std::hash<K>, UniversalTransparentEquality<K>>;

}

#endif // OPTIFOL_HPP
