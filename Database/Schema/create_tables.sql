/*
 * Copyright (c) All Rights Reserved
 * 2025 Oliver Dixon <od641@york.ac.uk>
 */

CREATE TABLE project
(
    id            INTEGER PRIMARY KEY GENERATED ALWAYS AS IDENTITY,
    name          TEXT      NOT NULL UNIQUE,
    created_at    TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    last_modified TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE subsystem
(
    id            INTEGER PRIMARY KEY GENERATED ALWAYS AS IDENTITY,
    project_id    INTEGER   NOT NULL REFERENCES project (id) ON DELETE CASCADE,
    name          TEXT      NOT NULL,
    created_at    TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    last_modified TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE requirement
(
    id            INTEGER PRIMARY KEY GENERATED ALWAYS AS IDENTITY,
    subsystem_id  INTEGER   NOT NULL REFERENCES subsystem (id) ON DELETE CASCADE,
    name          TEXT      NOT NULL,
    created_at    TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    last_modified TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    sentence      TEXT      NOT NULL,
    test_id       INTEGER REFERENCES test (id) ON DELETE CASCADE,
    priority      INTEGER   NOT NULL
);

CREATE TABLE stakeholder
(
    id   INTEGER PRIMARY KEY GENERATED ALWAYS AS IDENTITY,
    name TEXT NOT NULL
);

CREATE TABLE stakeholder_requirement
(
    id             INTEGER PRIMARY KEY GENERATED ALWAYS AS IDENTITY,
    stakeholder_id INTEGER NOT NULL REFERENCES stakeholder (id) ON DELETE CASCADE,
    requirement_id INTEGER NOT NULL REFERENCES requirement (id) ON DELETE CASCADE
);

CREATE TABLE test
(
    id            INTEGER PRIMARY KEY GENERATED ALWAYS AS IDENTITY,
    test_suite    TEXT      NOT NULL,
    note          TEXT,
    created_at    TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    last_modified TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    last_executed TIMESTAMP,
    last_result   BOOLEAN
);
