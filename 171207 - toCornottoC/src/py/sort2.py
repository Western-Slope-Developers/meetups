import sys

lines = open(sys.argv[1]).readlines()
lines.sort()
map(sys.stdout.write, lines)
