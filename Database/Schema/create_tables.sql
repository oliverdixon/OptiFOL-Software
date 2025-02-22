CREATE TABLE project
(
    id            INTEGER PRIMARY KEY GENERATED ALWAYS AS IDENTITY,
    name          TEXT      NOT NULL UNIQUE,
    created_at    TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    last_modified TIMESTAMP          DEFAULT NULL
);

CREATE TABLE subsystem
(
    id            INTEGER PRIMARY KEY GENERATED ALWAYS AS IDENTITY,
    project_id    INTEGER   NOT NULL REFERENCES project (id) ON DELETE CASCADE,
    name          TEXT      NOT NULL UNIQUE,
    created_at    TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    last_modified TIMESTAMP          DEFAULT NULL
);

CREATE TABLE requirement
(
    id            INTEGER PRIMARY KEY GENERATED ALWAYS AS IDENTITY,
    subsystem_id  INTEGER   NOT NULL REFERENCES subsystem (id) ON DELETE CASCADE,
    name          TEXT      NOT NULL UNIQUE,
    created_at    TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    last_modified TIMESTAMP          DEFAULT NULL,
    sentence      TEXT      NOT NULL
);

CREATE TABLE stakeholder
(
    id   INTEGER PRIMARY KEY GENERATED ALWAYS AS IDENTITY,
    name TEXT NOT NULL UNIQUE
);

CREATE TABLE stakeholder_requirement
(
    id             INTEGER PRIMARY KEY GENERATED ALWAYS AS IDENTITY,
    stakeholder_id INTEGER NOT NULL REFERENCES stakeholder (id) ON DELETE CASCADE,
    requirement_id INTEGER NOT NULL REFERENCES requirement (id) ON DELETE CASCADE
);
