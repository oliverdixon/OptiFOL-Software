# Optifol Software

`uk.ac.york.www_users.od641.optifol`

## CMake Presets

A single CMake preset has been defined to support `vcpkg`-provided shared libraries on x86-64 Linux systems:
`x64-linux-dynamic-vcpkg`. Users wishing to build *Optifol* under this configuration should inherit the preset
with a  user profile that specifies the absolute path of the `vcpkg` installation root. The path should be
given by exporting the `VCPKG_ROOT` environment variable.

The standard CMake configurations provided in this repository use the preset named `default`.

### Example `CMakeUserPresets.json`

```json
{
  "version": 3,
  "configurePresets": [
    {
      "name": "default",
      "inherits": "x64-linux-dynamic-vcpkg",
      "environment": {
        "VCPKG_ROOT": "/home/owd/.vcpkg-clion/vcpkg/"
      }
    }
  ]
}
```

Copyright (C) 2025 [Oliver Dixon](mailto:Oliver%20Dixon%20<od641@york.ac.uk>).
All rights reserved.
