/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

-- Insert projects and preserve their generated IDs
WITH inserted_projects AS (
    INSERT INTO project (name)
        VALUES ('Astute'),
               ('Vanguard'),
               ('Dreadnought')
        RETURNING id)

-- Use the returned IDs to insert subsystems
INSERT INTO subsystem (project_id, name)
SELECT inserted_projects.id, names.subsystem_name
FROM inserted_projects,
     (VALUES ('Sonar'), ('Comms'), ('Propulsion')) AS names(subsystem_name)
ORDER BY inserted_projects.id;
