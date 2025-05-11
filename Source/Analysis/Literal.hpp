/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class specification for the SAT Engine literal
 * @author Oliver Dixon
 * @date 2025-05-11
 * @version Development
 */

#ifndef LITERAL_HPP
#define LITERAL_HPP

#include <log4cxx/logger.h>

namespace optifol
{

class Literal {
public:
    enum class WatchState
    {
        NotWatching = 0,
        BackwardWatch = 1,
        Indeterminate = 2,
        ForwardWatch = 3
    };

    explicit Literal(u_int32_t variable_id, bool negative = false);

    [[nodiscard]] WatchState get_watch() const noexcept;

    void set_watch(WatchState watch_state) noexcept;

    [[nodiscard]] u_int32_t get_variable_id() const noexcept;

    [[nodiscard]] u_int32_t get_signed_variable_id() const noexcept;

    [[nodiscard]] bool is_negative() const noexcept;

    void write_log_entry() const;

private:
    static log4cxx::LoggerPtr logger;

    /**
     * @brief Memory-efficient encoding of literal information.
     * @details The semantics of the 32 bits are segmented as follows, counting from the MSB to the LSB:
     *  <ul>
     *      <li>[32, 4]: the unique ID of the variable of which the literal is an instantiation.</li>
     *      <li>[ 3, 2]: the watch-state of the literal; see the WatchState enumerator.</li>
     *      <li>( 2, 1]: the polarity of the literal: 0 for positive; 1 for negative.</li>
     *  </ul>
     *  Note that due to this layout, there is a maximum limit of 2**28 variables for the SAT engine. That should be
     *  plenty!
     */
    u_int32_t value;
};

}

#endif
