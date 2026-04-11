/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the ISerialisable interface
 * @author Oliver Dixon
 * @date 2026-01-25
 * @version Development
 */

#ifndef OPTIFOL_ISERIALISABLE_HPP
#define OPTIFOL_ISERIALISABLE_HPP

#include <format>
#include <sstream>

namespace optifol
{

/**
 * @class ISerialisable
 * @brief An interface for objects that may be serialised to a @ref std::ostream
 */
class ISerialisable
{
public:
    /**
     * @brief Destruct the serialisable object
     */
    virtual ~ISerialisable() = default;

    /**
     * @brief Serialise the object to the given output stream
     * @param ostream The destination output stream
     * @return The populated output stream
     */
    virtual std::ostream &serialise(std::ostream &ostream) const = 0;

    /**
     * @brief Operator overload to serialise the object to the given output stream
     * @param ostream The destination output stream
     * @param object The object to serialise
     * @return The populated output stream
     */
    friend std::ostream &operator<<(std::ostream &ostream, const ISerialisable &object)
    {
        return object.serialise(ostream);
    }
    
    ISerialisable(const ISerialisable&) = default;
    ISerialisable& operator=(const ISerialisable&) = default;
    ISerialisable(ISerialisable&&) = default;
    ISerialisable& operator=(ISerialisable&&) = default;

protected:
    ISerialisable() = default;
};

} // namespace optifol

// ReSharper disable once CppDoxygenUnresolvedReference

/**
 * @class formatter<Serialisable>
 * @brief Helper for C++20 @ref std::format support on serialisable Optifol types.
 * @tparam Serialisable The derived ISerialisable type to format
 */
template<typename Serialisable> requires std::derived_from<Serialisable, optifol::ISerialisable>
struct std::formatter<Serialisable> : formatter<string> // NOLINT(*-dcl58-cpp)
{
    /**
     * @brief Format the given sentence according to the implementation-defined serialiser.
     * @param value The Serialisable to serialise with the <code>operator&lt;&lt;</code> call.
     * @param context The streamed formatting context
     * @return The updated formatting context
     */
    auto format(const Serialisable &value, format_context &context) const
    {
        ostringstream output_stream;
        output_stream << value;
        return formatter<string>::format(output_stream.str(), context);
    }
};

#endif // OPTIFOL_ISERIALISABLE_HPP
