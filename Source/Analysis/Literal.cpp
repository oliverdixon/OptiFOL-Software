/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

/**
 * @file
 * @brief Class implementation for the SAT Engine literal
 * @author Oliver Dixon
 * @date 2025-05-11
 * @version Development
 */

#include "Literal.hpp"

#include <bitset>

#include "../Logging.hpp"

namespace optifol
{

log4cxx::LoggerPtr Literal::logger = Logging::get_logger({"LogicServices", "SATEngine", "Literal"});

Literal::Literal(const u_int32_t variable_id, const bool negative)
    : value(variable_id << 0x03 | negative & 0x01)
{
    logger->debug("Created literal with layout: " + std::bitset<32>(value).to_string());
    write_log_entry();
}

Literal::WatchState Literal::get_watch() const noexcept
{
    return static_cast<WatchState>((value & 0x06) >> 0x01);
}

void Literal::set_watch(WatchState watch_state) noexcept
{
    value |= static_cast<u_int32_t>(static_cast<std::underlying_type_t<WatchState>>(watch_state)) << 0x01;
}

u_int32_t Literal::get_variable_id() const noexcept
{
    return value >> 0x03;
}

u_int32_t Literal::get_signed_variable_id() const noexcept
{
    return value >> 0x02;
}

bool Literal::is_negative() const noexcept
{
    return value & 0x01;
}

void Literal::write_log_entry() const
{
    logger->debug("Literal encodes variable ID " + std::to_string(get_variable_id()) + " with " +
        (is_negative() ? "negative" : "positive") + " polarity and unset watch-state.");
}

} // namespace optifol
