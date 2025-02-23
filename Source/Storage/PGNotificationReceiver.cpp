/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

#include "PGNotificationReceiver.hpp"
#include "../Exceptions/BadStorageNotificationException.hpp"

namespace optifol
{

PGNotificationReceiver::PGNotificationReceiver(pqxx::connection &connection,
                                         const std::string_view entity,
                                         const NotificationPayload::PayloadType payload_type,
                                         sigc::slot<signal_signature>&& signal_slot):
    pqxx::notification_receiver(connection, construct_channel_name(entity, payload_type)),
    payload_type(payload_type)
{
    signal_connection = signal.connect(std::move(signal_slot));
}

void PGNotificationReceiver::operator()(const std::string &payload, int backend_pid)
{
    std::size_t target_id;

    try {
        target_id = std::stoul(payload);
    } catch (const std::invalid_argument& exception) {
        throw BadStorageNotificationException(exception.what());
    } catch (const std::out_of_range& exception) {
        throw BadStorageNotificationException(exception.what());
    }

    signal.emit(NotificationPayload(target_id));
}

std::string PGNotificationReceiver::construct_channel_name(const std::string_view entity,
                                                           const NotificationPayload::PayloadType payload_type)
{
    std::string action_string;

    switch (payload_type) {
    case NotificationPayload::PayloadType::Insert:
        action_string = "_insert";
        break;
    case NotificationPayload::PayloadType::Update:
        action_string = "_update";
        break;
    case NotificationPayload::PayloadType::Delete:
        action_string = "_delete";
        break;
    }

    return std::string(entity) + action_string;
}

}
