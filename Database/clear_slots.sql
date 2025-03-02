/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

DO
$$
    DECLARE
        target_slot TEXT;
    BEGIN
        FOR target_slot IN
            SELECT slot_name FROM pg_replication_slots
            LOOP
                EXECUTE 'SELECT ''stop'' FROM pg_drop_replication_slot(''' || target_slot || ''')';
            END LOOP;
    END
$$;
