#! /usr/bin/env python

import sys

input_file = sys.argv[1]
output_file = sys.argv[2]

output = open(output_file, 'w')

with open(input_file, 'r') as f:
    for line in f:
        if line.strip() and not line.startswith('#'):
            data = line.split()
            if data[1] == 'org:file_load':
                # TODO: this is the first organism, what about the parent?
                data[3] = '0'
            fields = data[0], data[3], data[11], data[12], data[13], data[16]
            output.write(" ".join(fields) + '\n')

output.close()
