#!/bin/env python3

import os
import sys
import re

success = True

if os.system('make') == 0:
	print("Running tests...", file = sys.stderr)
	os.system('bin/release/spasm')

	with open("bytes.txt") as bytes, open("instructions.txt") as instructions:

		byte_lines = bytes.readlines()
		instructions_lines = instructions.readlines()

		if os.system('as instructions.txt -o instructions.bin') != 0:
			sys.exit(1)

		i = 0
		
		for line in os.popen('objdump -w -M intel -d instructions.bin'):
			match = re.match( r'.*:\s(([a-f0-9]{2}\s)+)\s+mov.*\n', line)
			if (match):
				if (match.group(1).strip() != byte_lines[i].strip()):
					
					# i + 1 because we have the Intel syntax declaration on the first line
					print(line.strip(), "Instead it is: ", byte_lines[i].strip(), instructions_lines[i + 1].strip(), file = sys.stderr) 
					success = False

				i += 1

	os.unlink("bytes.txt")
	os.unlink("instructions.bin")
	os.unlink("instructions.txt")

if not success:
	print("Some tests failed. See above for details.", file = sys.stderr) 
	sys.exit(1)
else:
	print("All tests succeded! :)", file = sys.stderr) 