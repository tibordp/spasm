#!/bin/env python3
import os
import sys

with open('main.s', 'w') as out:
	print('.intel_syntax noprefix', file = out)
	while sys.stdin:
		line = sys.stdin.readline()
		if line.strip() == '.':
			break
		else:
			print(line, file=out) 

	
if os.system('as main.s -n --64 -o main.bin') == 0:
	for line in os.popen('objdump -w -M intel -d main.bin'):
		print (line.strip())

	os.unlink('main.s')
	os.unlink('main.bin')
