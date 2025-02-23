/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

-- TODO: I really doubt that this Cartesian product style of declaration is the best way to achieve this.

CREATE FUNCTION notify_on_delete()
    RETURNS TRIGGER AS $$
BEGIN
    -- Send a notification to the channel identified by the entity name followed by '_delete'.
    -- Note that the entity name must be passed as the first argument to the trigger function.
    PERFORM pg_notify(TG_ARGV[0] || '_delete', OLD.id::text);
    RETURN OLD;
END;
$$ LANGUAGE plpgsql;

CREATE FUNCTION notify_on_update()
    RETURNS TRIGGER AS $$
BEGIN
    -- Send a notification to the channel identified by the entity name followed by '_update'.
    -- Note that the entity name must be passed as the first argument to the trigger function.
    PERFORM pg_notify(TG_ARGV[0] || '_update', NEW.id::text);
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE FUNCTION notify_on_insert()
    RETURNS TRIGGER AS $$
BEGIN
    -- Send a notification to the channel identified by the entity name followed by '_insert'.
    -- Note that the entity name must be passed as the first argument to the trigger function.
    PERFORM pg_notify(TG_ARGV[0] || '_insert', NEW.id::text);
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

-- noinspection SqlSignature @ routine/"notify_on_delete"
CREATE TRIGGER trigger_notify_on_delete
    AFTER DELETE
    ON Subsystem
    FOR EACH ROW
EXECUTE FUNCTION notify_on_delete('subsystem');

-- noinspection SqlSignature @ routine/"notify_on_delete"
CREATE TRIGGER trigger_notify_on_delete
    AFTER DELETE
    ON Project
    FOR EACH ROW
EXECUTE FUNCTION notify_on_delete('project');

-- noinspection SqlSignature @ routine/"notify_on_update"
CREATE TRIGGER trigger_notify_on_update
    AFTER UPDATE
    ON Project
    FOR EACH ROW
EXECUTE FUNCTION notify_on_update('project');

-- noinspection SqlSignature @ routine/"notify_on_update"
CREATE TRIGGER trigger_notify_on_update
    AFTER UPDATE
    ON Subsystem
    FOR EACH ROW
EXECUTE FUNCTION notify_on_update('subsystem');

-- noinspection SqlSignature @ routine/"notify_on_insert"
CREATE TRIGGER trigger_notify_on_insert
    AFTER INSERT
    ON Project
    FOR EACH ROW
EXECUTE FUNCTION notify_on_insert('project');

-- noinspection SqlSignature @ routine/"notify_on_insert"
CREATE TRIGGER trigger_notify_on_insert
    AFTER INSERT
    ON Subsystem
    FOR EACH ROW
EXECUTE FUNCTION notify_on_insert('subsystem');
