from collections import defaultdict

reg_registers = [
	{ 'attrs' : {'type' : 'r8', 'rex.r' : False },  'registers' : [ 'al', 'cl', 'dl', 'bl', 'ah', 'ch', 'dh', 'bh' ]},
	{ 'attrs' : {'type' : 'r8', 'rex.r' : False },  'registers' : [  None, None, None, None, 'spl', 'bpl', 'sil', 'dil', ]},
	{ 'attrs' : {'type' : 'r8', 'rex.r' : True },   'registers' : ['r8b', 'r9b', 'r10b', 'r11b', 'r12b', 'r13b', 'r14b', 'r15b'] },
	{ 'attrs' : {'type' : 'r16', 'rex.r' : False }, 'registers' : ['ax', 'cx', 'dx', 'bx', 'sp', 'bp', 'si', 'di', ]},
	{ 'attrs' : {'type' : 'r16', 'rex.r' : True },  'registers' : ['r8w', 'r9w', 'r10w', 'r11w', 'r12w', 'r13w', 'r14w', 'r15w'] },
	{ 'attrs' : {'type' : 'r32', 'rex.r' : False }, 'registers' : ['eax', 'ecx', 'edx', 'ebx', 'esp', 'ebp', 'esi', 'edi', ]},
	{ 'attrs' : {'type' : 'r32', 'rex.r' : True },  'registers' : ['r8d', 'r9d', 'r10d', 'r11d', 'r12d', 'r13d', 'r14d', 'r15d'] },
	{ 'attrs' : {'type' : 'r64', 'rex.r' : False }, 'registers' : ['rax', 'rcx', 'rdx', 'rbx', 'rsp', 'rbp', 'rsi', 'rdi', ]},
	{ 'attrs' : {'type' : 'r64', 'rex.r' : True },  'registers' : ['r8', 'r9', 'r10', 'r11', 'r12', 'r13', 'r14', 'r15'] },
	{ 'attrs' : {'type' : 'mm', 'rex.r' : False },  'registers' : ['mm0', 'mm1', 'mm2', 'mm3', 'mm4', 'mm5', 'mm6', 'mm7', ]},
	{ 'attrs' : {'type' : 'xmm', 'rex.r' : False }, 'registers' : ['xmm0', 'xmm1', 'xmm2', 'xmm3', 'xmm4', 'xmm5', 'xmm6', 'xmm7', ]},
	{ 'attrs' : {'type' : 'xmm', 'rex.r' : True },  'registers' : ['xmm8', 'xmm9', 'xmm10', 'xmm11', 'xmm12', 'xmm13', 'xmm14', 'xmm15'] },
	{ 'attrs' : {'type' : 'sreg', 'rex.r' : False },  'registers' : ['es', 'cs', 'ss', 'ds', 'fs', 'gs', None, None, ]},
	{ 'attrs' : {'type' : 'eee', 'rex.r' : False },  'registers' : ['cr0', 'cr1', 'cr2', 'cr3', 'cr4', 'cr5', 'cr6', 'cr7']},
	{ 'attrs' : {'type' : 'eee', 'rex.r' : True },  'registers' : ['cr8', 'cr9', 'cr10', 'cr11', 'cr12', 'cr13', 'cr14', 'cr15'] },
	{ 'attrs' : {'type' : 'eee', 'rex.r' : False },  'registers' : ['dr0', 'dr1', 'dr2', 'dr3', 'dr4', 'dr5', 'dr6', 'dr7']},
	{ 'attrs' : {'type' : 'eee', 'rex.r' : True },  'registers' : ['dr8', 'dr9', 'dr10', 'dr11', 'dr12', 'dr13', 'dr14', 'dr15'] } ,
	
	{ 'attrs' : {'type' : 'mm', 'rex.r' : True },   'registers' : ['mm0', 'mm1', 'mm2', 'mm3', 'mm4', 'mm5', 'mm6', 'mm7'] },
	{ 'attrs' : {'type' : 'sreg', 'rex.r' : True },  'registers' : ['es', 'cs', 'ss', 'ds', 'fs', 'gs', None, None] },
]

used_elements = set()
attributes = defaultdict(int)
values = dict()

types = { 
	'r8'  : 0,	
	'r16' : 1,	
	'r32' : 2,	
	'r64' : 3,	
	'mm'  : 4,	
	'xmm' : 5,	
	'sreg': 6,	 
	'eee' : 7 
}

for idx, i in enumerate(reg_registers):
	print ("\t", end = '')
	for idx2, j in enumerate(i['registers']):
		if (j is not None) and (j not in used_elements):
			value = idx * 8 + idx2
			values[j] = value
			print ("%s = %s, " % (j, value), end = '')
			used_elements.add(j)
		if (j is not None):
			if (attributes[values[j]] == 0):
				attributes[values[j]] = 2 if i['attrs']['rex.r'] else 1
			else:
				attributes[values[j]] = 3 
			attributes[values[j]] |= (types[i['attrs']['type']] << 2)
	print()


for i in range(len(used_elements)):
	if (i % 8 == 0):
		print()
	print(str(attributes[i]) + ', ', end='')

for index, i in enumerate(used_elements):
	if (index % 5 == 0):
		print()
	print ("{ R::%s, \"%s\" }," % (i,i), end='')