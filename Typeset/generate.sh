#!/usr/bin/env bash

#
# OWD 2026
#
# Produces LaTeX commands to replicate a directory hierarchy in a document
# structure. Files of known extensions (see $LANGUAGES) are typeset with the
# '\lstinputlisting' command from the 'listings' package. File paths in the
# output are specified relative to the location of this script ($0).
#
# The enclosing document should define all sectional commands specified in
# $SECTION_NAMES. The listings language 'PlainText' should also be defined as
# the empty language. Note that the number of entries in $SECTION_NAMES dictates
# the maximum depth of the root directory structure.
#
# There are no fatal errors, but warnings will be described on stderr and the
# process will exit with a non-zero return code if any warnings were issued
# during execution.
#
# Users may wish to post-process stdout to escape TeX-sensitive characters
# likely to occur in filenames, e.g. [...] | sed 's/_/\\_/g'.
#
# Syntax: $0 [root directory...] [individual file...]
#

shopt -s nullglob

SCRIPT_BASE="$(cd -- "$(dirname -- "$0")" && pwd -P)"

declare -a SECTION_NAMES=(
    "\section"
    "\subsection"
    "\subsubsection"
    "\paragraph"
    "\subparagraph"
    "\subsubparagraph"
)

declare -A LANGUAGES=(
    [cpp]="C++"
    [hpp]="C++"
    [y]="C++"
    [l]="PlainText"
    [ui]="XML"
    [xml]="XML"
    [gresource.xml.in]="XML"
    [ui.in]="XML"
    [txt]="PlainText"
    [html]="HTML"
    [awk]="Awk"
    [tex]="TeX"
    [css]="PlainText"
    [json]="PlainText"
    [md]="PlainText"
)

declare -A LANGUAGES_OVERRIDE=(
    [CMakeLists.txt]="CMake"
    [Doxyfile]="PlainText"
    [.gitignore]="PlainText"
    [.clang-format]="PlainText"
)

depth=0
status=0

#
# Write an entry (heading and listing) for a single file.
#
# $1: target file path
#
write_file() {
    local BASENAME="$(basename "$1")"

    # Lookup the filename in the language override table.
    local lang_type="${LANGUAGES_OVERRIDE["$BASENAME"]}"
    if [[ -z "$lang_type" ]]; then
        # If no override applies, use the extension.
        local FILE_EXT="${BASENAME#*.}"
        lang_type="${LANGUAGES["$FILE_EXT"]}"
        if [[ -z "$lang_type" ]]; then
            >&2 echo "Skipping $1 due to unknown extension '$FILE_EXT'."
            status=1
            return
        fi
    fi
    
    # Issue the section and listings command, the latter pointing to the $1 file
    # relative to the location of this script.
    printf "%s{%s}\n\lstinputlisting[language=%s]{%s}\n" \
        "${SECTION_NAMES[$depth]}" \
        "$BASENAME" \
        "$lang_type" \
        "$(realpath --relative-to="$SCRIPT_BASE" "$1")"
}

#
# Recursively visit a directory, issuing section commands for subdirectories and
# issuing '\lstinputlisting' commands for files.
#
# $1: root directory path
#
visit() {
    local DIR=$1
    local BASENAME="$(basename "$DIR")"

    if (( depth + 1 >= ${#SECTION_NAMES[@]} )); then
        >&2 echo "Skipping $DIR due to excessive depth."
        status=1
        return
    fi

    printf "%s{%s}\n" "${SECTION_NAMES[$depth]}" "$BASENAME"
    ((depth++))

    local filename
    local child

    for filename in "$DIR"/*; do
        [[ -f "$filename" ]] || continue
        write_file "$filename"
    done

    for child in "$DIR"/*; do
        [[ -d "$child" && ! -L "$child" ]] || continue
        visit "$child"
    done

    ((depth--))
}

#
# Entry point: process each directory or file specified on the command line.
#
for target in "$@"; do
    if [[ -d "$target" ]]; then    
        visit "$target"
    elif [[ -f "$target" ]]; then
        write_file "$target"
    else
        status=1
        >&2 echo "Skipping $target since it is not a file nor a directory."
    fi
done

exit $status
