/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

INSERT INTO project (name)
VALUES ('Astute'),
       ('Vanguard'),
       ('Dreadnought');

INSERT INTO subsystem (project_id, name)
VALUES (8, 'Sonar'),
       (8, 'Comms'),
       (8, 'Propulsion'),
       (9, 'Sonar'),
       (9, 'Comms'),
       (9, 'Propulsion'),
       (10, 'Sonar'),
       (10, 'Comms'),
       (10, 'Propulsion');
