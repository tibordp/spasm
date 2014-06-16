import modrm
import sys
import re
import itertools
import os

all_registers = [
	'ah', 'al', 'ax', 'bh', 'bl', 'bp', 'bpl', 'bx', 'ch', 'cl', 
	#'cr0', 'cr1', 'cr10', 'cr11', 'cr12', 'cr13', 'cr14', 'cr15', 'cr2', 'cr3', 'cr4', 'cr5', 'cr6', 'cr7', 'cr8', 'cr9', 
	'cs', 'cx', 'dh', 'di', 'dil', 'dl', 
	#'dr0', 'dr1', 'dr10', 'dr11', 'dr12', 'dr13', 'dr14', 'dr15', 'dr2', 'dr3', 'dr4', 'dr5', 'dr6', 'dr7', 'dr8','dr9', 
	'ds', 'dx', 'eax', 'ebp', 'ebx', 'ecx', 'edi', 'edx', 'es', 'esi', 'esp',
	'fs', 'gs', 
	#'mm0', 'mm1', 'mm2', 'mm3', 'mm4', 'mm5', 'mm6', 'mm7', 
	'r10', 'r10b',
	'r10d', 'r10w', 'r11', 'r11b', 'r11d', 'r11w', 'r12', 'r12b', 'r12d', 'r12w', 'r13',
	'r13b', 'r13d', 'r13w', 'r14', 'r14b', 'r14d', 'r14w', 'r15', 'r15b', 'r15d', 'r15w',
	'r8', 'r8b', 'r8d', 'r8w', 'r9', 'r9b', 'r9d', 'r9w', 'rax', 'rbp', 'rbx', 'rcx', 'rdi',
	'rdx', 'rsi', 'rsp', 'si', 'sil', 'sp', 'spl', 'ss'
	 #'xmm0', 'xmm1', 'xmm10', 'xmm11', 'xmm12', 'xmm13', 'xmm14', 'xmm15', 'xmm2', 'xmm3', 'xmm4', 'xmm5', 'xmm6', 'xmm7', 'xmm8', 'xmm9'
]

def test_sdp_single():
	out_asm    = open('main.s',  'w')
	ops = []

	print('.intel_syntax noprefix', file = out_asm)

	for index, (a,b,multiplier) in enumerate(itertools.product(all_registers, all_registers,  [1,2,4,8])):
		if index % 1000 == 0:
			print ("Index: %d" % (index, ), file=sys.stderr)

		for i in modrm.get([multiplier ,a], b):
			displacements = { 32 : '0x0eadbeef', 8 : '0x01', 0 : '0x00' }
			disp = displacements[i["add_displacement"]]

			print( "mov [%d*%s + %s], %s" % (multiplier, a, disp, b), file = out_asm)
			ops.append( (i["add_displacement"],  ([multiplier ,a], b),  "%s %s" % (i['mod_rm'], i['sib']) ))

	out_asm.close()

	os.system('as main.s -o main.bin')

	i = 0

	for line in os.popen('objdump -w -M intel -d main.bin'):
		(displacement, operands, text) = ops[i]

		regexes = { 
			32 : r'.*\s([a-f0-9]{2}\s[a-f0-9]{2})\sef\sbe\sad\s0e\s+mov.*\n', 
			8 : r'.*\s([a-f0-9]{2}\s[a-f0-9]{2})\s01\s+mov.*\n',
			0 : r'.*\s([a-f0-9]{2}\s[a-f0-9]{2})\s+mov.*\n' 
		}

		match = re.match(regexes[displacement], line)

		if match:
			if (match.group(1) != text ):
				return operands
			i += 1

	os.unlink('main.s')
	os.unlink('main.bin')
	return None

import random

def test_sdp_multiple():
	out_asm    = open('main.s',  'w')
	ops = []

	print('.intel_syntax noprefix', file = out_asm)

	for index, (a,b,c,multiplier) in enumerate(itertools.product(all_registers, all_registers, all_registers,  [1,2,4,8])):
		if index % 1000 == 0:
			print ("Index: %d" % (index, ), file=sys.stderr)

		for i in modrm.get([multiplier ,a, c], b):
			displacements = { 32 : '0x0eadbeef', 8 : '0x01', 0 : '0x00' }
			disp = displacements[i["add_displacement"]]

			print( "mov [%d*%s + %s + %s], %s" % (multiplier, a, c, disp, b), file = out_asm)
			ops.append( (i["add_displacement"],  ([multiplier ,a, c], b),  "%s %s" % (i['mod_rm'], i['sib']) ))


	out_asm.close()

	os.system('as main.s -o main.bin')

	i = 0

	for line in os.popen('objdump -w -M intel -d main.bin'):
		(displacement, operands, text) = ops[i]

		regexes = { 
			32 : r'.*\s([a-f0-9]{2}\s[a-f0-9]{2})\sef\sbe\sad\s0e\s+mov.*\n', 
			8 : r'.*\s([a-f0-9]{2}\s[a-f0-9]{2})\s01\s+mov.*\n',
			0 : r'.*\s([a-f0-9]{2}\s[a-f0-9]{2})\s+mov.*\n' 
		}

		match = re.match(regexes[displacement], line)

		if match:
			if (match.group(1) != text ):
				return operands
			i += 1

	os.unlink('main.s')
	os.unlink('main.bin')
	return None


tests = ['test_sdp_single', 'test_sdp_multiple']

print(modrm.get([1, 'eax', 'eax'], 'ah'))

for test in (tests):
	result = eval(test + '()')
	if result is None:
		print("Tests passed!")
	else:
		print("Test %s failed at: " % test, result)