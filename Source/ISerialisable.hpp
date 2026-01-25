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

class ISerialisable
{
public:
    virtual ~ISerialisable() = default;

    virtual std::ostream &serialise(std::ostream &ostream) const = 0;

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
 * @class formatter<SentenceType>
 * @brief Helper for C++20 @ref std::format support on serialisable Optifol types.
 * @tparam SentenceType The derived ISerialisable type to format
 */
template<typename SentenceType> requires std::derived_from<SentenceType, optifol::ISerialisable>
struct std::formatter<SentenceType> : formatter<string> // NOLINT(*-dcl58-cpp)
{
    /**
     * @brief Format the given sentence according to the implementation-defined serialiser.
     * @param value The SentenceType to serialise with the <code>operator&lt;&lt;</code> call.
     * @param context The streamed formatting context
     * @return The updated formatting context
     */
    auto format(const SentenceType &value, format_context &context) const
    {
        ostringstream output_stream;
        output_stream << value;
        return formatter<string>::format(output_stream.str(), context);
    }
};

#endif // OPTIFOL_ISERIALISABLE_HPP
