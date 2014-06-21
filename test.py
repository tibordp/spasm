#!/bin/env python3


#    spasm - x86-64 assembler / JIT support library
#    Copyright (C) 2014  Tibor Djurica Potpara <tibor.djurica@ojdip.net>
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.


import os
import random
import sys
import re

success = True

print("Running tests...", file = sys.stderr)
if os.system('bin/release/spasm'):
	success = False
else:
	with open("bytes.txt") as bytes, open("instructions.txt") as instructions:
		byte_lines = bytes.readlines()
		instructions_lines = instructions.readlines()

	if os.system('as instructions.txt -o instructions.bin') != 0:
		sys.exit(1)

	i = 0
	for line in os.popen('objdump -w -M intel -d instructions.bin'):
		match = re.match( r'.*:\s(([a-f0-9]{2}\s)+)\s+(mov.*)\n', line)
		if (match):
			#if ((match.group(1).strip() != byte_lines[i].strip()) or random.choice(range(1000)) == 500):
			if (match.group(1).strip() != byte_lines[i].strip()):
				
				# i + 1 because we have the Intel syntax declaration on the first line
				print("Should be [", match.group(1).strip(), 
					"], instead it is: [", byte_lines[i].strip(), 
					"]", file = sys.stderr)
				print("\tinstruction: ",  instructions_lines[i + 1].strip(), file = sys.stderr) 
				print("\tdisassembly: ", match.group(3).strip(), file = sys.stderr) 
				success = False

			i += 1

try:
	os.unlink("bytes.txt")
	os.unlink("instructions.bin")
	os.unlink("instructions.txt")
except:
	pass

if not success:
	print("Some tests failed. See above for details.", file = sys.stderr) 
	sys.exit(1)
else:
	print("All tests succeded! :)", file = sys.stderr) 