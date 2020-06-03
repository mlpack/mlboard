#!/bin/bash
#
# Run Google's cpplint over the codebase and convert convert the output from
# Google's cpplint.py to the cppcheck XML format for consumption by the Jenkins
# cppcheck plugin.
#
# The given arguments are the src root and the reports directory e.g.:
# ./lint.sh --root . --reports reports/cpplint.xml

while [[ $# -gt 1 ]]
do
key="$1"

case $key in
    -r|--root)
    root="$2"
    shift # past argument
    ;;
    -e|--reports)
    reports="$2"
    shift # past argument
    ;;
    -d|--dir)
    dir="$2"
    shift # past argument
    ;;
    *)
          # unknown option
    ;;
esac
shift # past argument or value
done

linter="$(cd "$(dirname "${BASH_SOURCE[0]}" )" && pwd)"

# Change to source directory.
cd "$root"

# Get all files for the style check and exclude external files and run
# cpplint and convert the output.
find "$dir" \
! -path "*/include/proto/*" \
-print0 -iname '*.[hc]pp' -type f | \
xargs -0 python "$linter"/cpplint.py --extensions=hpp,cpp --filter=\
-whitespace/braces,\
-whitespace/newline,\
-build/header_guard,\
-build/include_order,\
-build/storage_class,\
-build/namespaces,\
-build/include_what_you_use,\
-legal/copyright,\
-readability/casting,\
-readability/alt_tokens,\
-readability/todo,\
-readability/multiline_string,\
-runtime/explicit,\
-runtime/int,\
-runtime/string,\
-runtime/references  

