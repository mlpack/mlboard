#!/usr/bin/env python

# Convert output from Google's cpplint.py to the cppcheck XML format for
# consumption by the Jenkins cppcheck plugin.

# Reads from stdin and writes to stderr (to mimic cppcheck)

import sys
import re
import xml.sax.saxutils

def cpplint_score_to_cppcheck_severity(score):
    # I'm making this up.
    if score == 1:
        return 'style'
    elif score == 2:
        return 'style'
    elif score == 3:
        return 'warning'
    elif score == 4:
        return 'warning'
    elif score == 5:
        return 'error'

def parse():
    sys.stderr.write('''<?xml version="1.0" encoding="UTF-8"?>\n''')
    sys.stderr.write('''<results version="2">\n''')
    sys.stderr.write('''<cppcheck version="1.66">\n''')
    sys.stderr.write('''<errors>\n''')

    # Do line-by-line conversion.
    r = re.compile('([^:]*):([0-9]*):  ([^\[]*)\[([^\]]*)\] \[([0-9]*)\].*')

    for l in sys.stdin.readlines():
        m = r.match(l.strip())
        if not m:
            continue

        g = m.groups()
        if len(g) != 5:
            continue

        fname, lineno, rawmsg, label, score = g
        msg = xml.sax.saxutils.escape(rawmsg, {'"' : "&quot;", "'" : "&apos;"}).strip()

        sys.stderr.write('''<error id="%s" severity="error" msg="%s" verbose="%s">\n'''%(label, msg, msg))
        sys.stderr.write('''<location file="%s" line="%s"/>\n'''%(fname, lineno))
        sys.stderr.write('''</error>\n''')

    # Write footer.
    sys.stderr.write('''</errors>\n''')
    sys.stderr.write('''</results>\n''')

if __name__ == '__main__':
    parse()
