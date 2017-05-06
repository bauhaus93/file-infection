#!/usr/bin/python2
import sys

def create_checksum(string):
    a = 1
    b = 0

    for c in string:
        a = (a + ord(c)) % 65521
        b = (b + a) % 65521#

    return ((b << 16) + a) & 0xFFFFFFFF;

argc = len(sys.argv)
if argc == 1:
    args = ["checksums.txt", "checksum_list.h", "CHECKSUM_LIST_H"]
elif argc != 4:
    print "Usage: filenameIn filenameOut safeguard"
    exit(1)
else:
    args = sys.argv[1:]

checksum = {}
with open(args[0], "r") as f:
    for line in f:
        name = line[:-2]    #\r\n
        cs = create_checksum(name)
        if cs in checksum:
            print "found hash collision: %s with %s -> 0x%08X" % (name, checksum[cs], cs)
            exit(2)
        checksum[cs] = name

l = []
for key in checksum.keys():
    l.append((checksum[key], key))
l.sort(key = lambda e: e[0])

with open(args[1], "w") as f:
    f.write("#ifndef %s\n#define %s\n" % (args[2], args[2]))
    for e in l:
        f.write("#define CS_%s (0x%X)\n" % (e[0].upper().replace(".", "_"), e[1]))
    f.write("#endif")

print "generated %s checksums" % len(l)
