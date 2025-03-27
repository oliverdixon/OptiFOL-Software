# Copyright (c) All Rights Reserved
# 2025 Oliver Dixon <od641@york.ac.uk>

# File: substitute.awk
# Brief: Perform K-V-based substitutions on files, whereby all instances of the key are replaced by the contents of file
#   whose path is detained by the value.
# Detail: The current working directory is presumed to contain a "replacements.txt" file containing zero or more key-
#   value pairs of the form 'key=value'; each separated by a newline. These lines are the only allowable entries in the
#   replacements file. For each instance of the key in the input stream, it is replaced by the contents of a file. The
#   path of that file is given by the value. If there are any invalid entries in the replacements file, a message is
#   produced on stderr and a non-zero exit code is emitted.
# Author: Oliver Dixon
# Date: 2025-03-26
# Version: Development

BEGIN {
    while ((getline < "./replacements.txt") > 0) {
        if (split($0, token, "=") != 2) {
            printf ("%s: %d: incorrect number of fields extracted") > "/dev/stderr"
            exit 1
        }

        PATTERNS[token[1]] = "\\$\\{" token[1] "\\}"
        VALUES[token[1]] = token[2]
    }

    close("./replacements.txt")
}

function read_file(filename, content, line) {
    content = ""

    while ((getline line < filename) > 0)
        content = content line "\n"

    close(filename)
    return content
}

{
    line = $0
    for (idx in PATTERNS)
        gsub(PATTERNS[idx], read_file(VALUES[idx]), line)

    print line
}
