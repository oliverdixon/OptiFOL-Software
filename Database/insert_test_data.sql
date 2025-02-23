/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

INSERT INTO project (name)
VALUES ('Astute'),
       ('Vanguard'),
       ('Dreadnought');

INSERT INTO subsystem (project_id, name)
VALUES (12, 'Sonar'),
       (12, 'Comms'),
       (12, 'Propulsion'),
       (13, 'Sonar'),
       (13, 'Comms'),
       (13, 'Propulsion'),
       (14, 'Sonar'),
       (14, 'Comms'),
       (14, 'Propulsion');
