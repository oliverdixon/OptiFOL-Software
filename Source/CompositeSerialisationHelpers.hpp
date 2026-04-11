/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Concept definitions for composite iterable-serialisable types
 * @author Oliver Dixon
 * @date 2025-06-15
 * @version Development
 */

#ifndef COMPOSITESERIALISABLECONCEPTS_HPP
#define COMPOSITESERIALISABLECONCEPTS_HPP

#include <iostream>
#include <iterator>

namespace optifol
{

/**
 * @concept SerialisableIterator
 * @brief Represents a bidirectional iterator to a de-referencable type with a @ref std::ostream serialisation
 *  function.
 * @details A SerialisableIterator satsifies the following constraints:
 *  <ul>
 *      <li>Compliant with @ref std::bidirectional_iterator;</li>
 *      <li>Be a constant iterator; and</li>
 *      <li>Iterates over a pointer-like type that can be dereferenced to a type with a serialiser member
 *          function.</li>
 *  </ul>
 */
template<typename Candidate>
concept SerialisableIterator = std::bidirectional_iterator<Candidate> &&
        std::is_const_v<std::remove_reference_t<decltype(*std::declval<Candidate>())>> &&
        requires(std::ostream &ostream, const Candidate &candidate) {
            { (*candidate)->serialise(ostream) } -> std::convertible_to<std::ostream &>;
        };

/**
 * @concept StringifiableIterator
 * @brief Represents a bidirectional iterator to a de-referencable type with a @ref std::string stringifier
 *  function.
 * @details A SerialisableIterator satsifies the following constraints:
 *  <ul>
 *      <li>Compliant with @ref std::bidirectional_iterator;</li>
 *      <li>Be a constant iterator; and</li>
 *      <li>Iterates over a pointer-like type that can be dereferenced to a type with a stringifier member
 *          function.</li>
 *  </ul>
 */
template<typename Candidate>
concept StringifiableIterator = std::bidirectional_iterator<Candidate> &&
        std::is_const_v<std::remove_reference_t<decltype(*std::declval<Candidate>())>> &&
        requires(std::ostream &ostream, const Candidate &candidate) {
            { (*candidate)->to_string() } -> std::convertible_to<std::string>;
        };

class CompositeSerialisationHelpers
{
public:
    /**
     * @brief Serialise a formatted symbol string, including all arguments, to the specified output stream.
     * @tparam Iterator @ref SerialisableIterator over the argument collection
     * @param ostream Destination output stream
     * @param display_name Symbol display name
     * @param arguments_begin Beginning iterator of the argument collection
     * @param arguments_end Ending iterator of the argument collection (one past last item)
     * @param is_negative_polarity Is the symbol instantiated with a negative polarity?
     * @return Populated destination output stream
     */
    template<SerialisableIterator Iterator>
    static std::ostream &stream_serialise(std::ostream &ostream, const std::string_view display_name,
            const Iterator arguments_begin, Iterator arguments_end, const bool is_negative_polarity = false)
    {
        if (is_negative_polarity)
            ostream << '~';

        ostream << display_name;

        if (arguments_begin == arguments_end)
            return ostream;

        ostream << '(';

        if (arguments_begin < arguments_end) {
            --arguments_end;

            for (auto argument_it = arguments_begin; argument_it != arguments_end; ++argument_it) {
                (*argument_it)->serialise(ostream);
                ostream << ", ";
            }

            (*arguments_end)->serialise(ostream);
        }

        return ostream << ')';
    }

    /**
     * @brief Serialise a formatted symbol string, including all arguments, into a dynamic @ref std::string
     *  buffer.
     * @tparam Iterator @ref StringifableIterator over the argument collection
     * @param display_name Symbol display name
     * @param arguments_begin Beginning iterator of the argument collection
     * @param arguments_end Ending iterator of the argument collection (one past last item)
     * @param is_negative_polarity Is the symbol instantiated with a negative polarity?
     * @return Populated string buffer
     */
    template<StringifiableIterator Iterator>
    static std::string string_serialise(std::string display_name, const Iterator arguments_begin,
            Iterator arguments_end, const bool is_negative_polarity = false)
    {
        std::string result;

        if (is_negative_polarity)
            result = '~';

        if (arguments_begin == arguments_end)
            return result + display_name;

        result += std::move(display_name) + '(';

        if (arguments_begin < arguments_end) {
            --arguments_end;

            for (auto argument_it = arguments_begin; argument_it != arguments_end; ++argument_it)
                result += (*argument_it)->to_string() + ", ";

            result += (*arguments_end)->to_string();
        }

        return result + ')';
    }
};

} // namespace optifol

#endif
