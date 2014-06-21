#!/bin/env python3

import os
import re

if os.system('make') == 0:

	os.system('bin/release/spasm')

	with open("bytes.txt") as bytes, open("instructions.txt") as instructions:

		i = 0

		byte_lines = bytes.readlines()
		instructions_lines = instructions.readlines()

		os.system('as instructions.txt -o instructions.bin')
		for line in os.popen('objdump -w -M intel -d instructions.bin'):
			match = re.match( r'.*:\s(([a-f0-9]{2}\s)+)\s+mov.*\n', line)
			if (match):
				if (match.group(1).strip() != byte_lines[i].strip()):
					print(line.strip(), "Instead it is: ", byte_lines[i].strip(), instructions_lines[i + 1].strip()) 

				i += 1

	os.unlink("bytes.txt")
	os.unlink("instructions.bin")
	#os.unlink("instructions.txt")
