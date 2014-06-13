from itertools import product

def sib_encode(scale_register, scale, offset_register, displacement = 32):
	offset_registers = [
		{ 'attrs' : { 'type' : 'r64', 'rex.b' : False }, 'registers' : ['rax', 'rcx', 'rdx', 'rbx', 'rsp', 'rbp', 'rsi', 'rdi' ] },
		{ 'attrs' : { 'type' : 'r64', 'rex.b' : True }, 'registers' : [ 'r8', 'r9', 'r10', 'r11', 'r12', 'r13', 'r14', 'r15'] },
		{ 'attrs' : { 'type' : 'r32', 'rex.b' : False }, 'registers' : ['eax', 'ecx', 'edx', 'ebx', 'esp', 'ebp', 'esi', 'edi' ] },
		{ 'attrs' : { 'type' : 'r32', 'rex.b' : True }, 'registers' : [ 'r8d', 'r9d', 'r10d', 'r11d', 'r12d', 'e13d', 'r14d', 'r15d'] }
	]

	special_sbp = { 'rbp', 'r13', 'ebp', 'e13d', None }
	

	scale_registers = [
		{ 'attrs' : {'rex.x' : False, 'type' : 'r64' },  'registers' : [ 'rax', 'rcx', 'rdx', 'rbx',   None, 'rbp', 'rsi', 'rdi'  ] },
		{ 'attrs' : {'rex.x' : False, 'type' : 'r32' },  'registers' : [ 'eax', 'ecx', 'edx', 'ebx',   None, 'ebp', 'esi', 'edi'  ] },
		{ 'attrs' : {'rex.x' : True,  'type' : 'r64' },  'registers' : [ 'r8',  'r9',  'r10',  'r11',  'r12', 'r13', 'r14', 'r15'  ] },
		{ 'attrs' : {'rex.x' : True,  'type' : 'r32' },  'registers' : [ 'r8d', 'r9d', 'r10d', 'r11d', 'r12d', 'r13d', 'r14d','r15d' ] }
	]

	scaled_matches = []

	for tentative_match in scale_registers:
		for index, reg in enumerate(tentative_match['registers']):
			if scale_register == reg:
				scaled_matches.append( (tentative_match['attrs'], index) )

	offset_matches = []

	for tentative_match in offset_registers:
		for index, reg in enumerate(tentative_match['registers']):
			if offset_register == reg:
				offset_matches.append( (tentative_match['attrs'], index) )

	for (scaled, index), (offset, o_index) in product(scaled_matches, offset_matches):
		
		scales = { 1 : 0, 2: 1, 4: 2, 8: 3 }
		modrm = (scales[scale]  << 6) | ( index << 3 ) | o_index;

		if offset_register is None:
			required_mod = 0
		elif displacement == 8:
			required_mod = 1
		else 
			required_mod = 2

		if offset_register not in special_sbp:
			required_mod = None

		yield { 
			'rex.x': scaled['rex.x'], 'rex.b': offset['rex.b'], 
			'scaled_type' : scaled['type'], 'offset_type' : offset['type'], 
			'modrm' : '%02x' % modrm  
			'required_mod' : required_mod,
		}				



def modrm_encode(dst_register, src_register, displacement = None, sib = False):
	destination_registers = [
		{ 'attrs' : {'type' : 'r8_norex', 'rex.r' : False },  'registers' : [ 'al', 'cl', 'dl', 'bl', 'ah', 'ch', 'dh', 'bh' ]},
		{ 'attrs' : {'type' : 'r8_norex', 'rex.r' : True },  'registers' : [None, None, None, None, None, None, None, None] },
		{ 'attrs' : {'type' : 'r8', 'rex.r' : False },  'registers' : ['al', 'cl', 'dl', 'bl', 'spl', 'bpl', 'sil', 'dil', ]},
		{ 'attrs' : {'type' : 'r8', 'rex.r' : True },  'registers' : ['r8b', 'r9b', 'r10b', 'r11b', 'r12b', 'r13b', 'r14b', 'r15b'] },
		{ 'attrs' : {'type' : 'r16', 'rex.r' : False },  'registers' : ['ax', 'cx', 'dx', 'bx', 'sp', 'bp', 'si', 'di', ]},
		{ 'attrs' : {'type' : 'r16', 'rex.r' : True },  'registers' : ['r8w', 'r9w', 'r10w', 'r11w', 'r12w', 'r13w', 'r14w', 'r15w'] },
		{ 'attrs' : {'type' : 'r32', 'rex.r' : False },  'registers' : ['eax', 'ecx', 'edx', 'ebx', 'esp', 'ebp', 'esi', 'edi', ]},
		{ 'attrs' : {'type' : 'r32', 'rex.r' : True },  'registers' : ['r8d', 'r9d', 'r10d', 'r11d', 'r12d', 'r13d', 'r14d', 'r15d'] },
		{ 'attrs' : {'type' : 'r64', 'rex.r' : False },  'registers' : ['rax', 'rcx', 'rdx', 'rbx', 'rsp', 'rbp', 'rsi', 'rdi', ]},
		{ 'attrs' : {'type' : 'r64', 'rex.r' : True },  'registers' : ['r8', 'r9', 'r10', 'r11', 'r12', 'r13', 'r14', 'r15'] },
		{ 'attrs' : {'type' : 'mm', 'rex.r' : False },  'registers' : ['mm0', 'mm1', 'mm2', 'mm3', 'mm4', 'mm5', 'mm6', 'mm7', ]},
		{ 'attrs' : {'type' : 'mm', 'rex.r' : True },  'registers' : ['mm0', 'mm1', 'mm2', 'mm3', 'mm4', 'mm5', 'mm6', 'mm7'] },
		{ 'attrs' : {'type' : 'xmm', 'rex.r' : False },  'registers' : ['xmm0', 'xmm1', 'xmm2', 'xmm3', 'xmm4', 'xmm5', 'xmm6', 'xmm7', ]},
		{ 'attrs' : {'type' : 'xmm', 'rex.r' : True },  'registers' : ['xmm8', 'xmm9', 'xmm10', 'xmm11', 'xmm12', 'xmm13', 'xmm14', 'xmm15'] },
		{ 'attrs' : {'type' : 'sreg', 'rex.r' : False },  'registers' : ['es', 'cs', 'ss', 'ds', 'fs', 'gs', None, None, ]},
		{ 'attrs' : {'type' : 'sreg', 'rex.r' : True },  'registers' : ['es', 'cs', 'ss', 'ds', 'fs', 'gs', None, None] },
		{ 'attrs' : {'type' : 'eee', 'rex.r' : False },  'registers' : ['cr0', None, 'cr2', 'cr3', 'cr4', None, None, None, ]},
		{ 'attrs' : {'type' : 'eee', 'rex.r' : True },  'registers' : ['cr8', None, None, None, None, None, None, None] },
		{ 'attrs' : {'type' : 'eee', 'rex.r' : False },  'registers' : ['dr0', 'dr1', 'dr2', 'dr3', 'dr41', 'dr51', 'dr6', 'dr7', ]},
		{ 'attrs' : {'type' : 'eee', 'rex.r' : True },  'registers' : [None, None, None, None, None, None, None, None] } 
	]

	source_registers = [
		{ 'attrs' : {'mod' : 0, 'rex.b' : False, 'displacement' : 0,   'type' : 'r64',},  'registers' : [ 'rax', 'rcx', 'rdx', 'rbx',  None,  None, 'rsi', 'rdi' ] },
		{ 'attrs' : {'mod' : 0, 'rex.b' : False, 'displacement' : 32,  'type' : 'r64',},  'registers' : [  None,  None,  None,  None,  None, 'rip',  None,  None ] },
		{ 'attrs' : {'mod' : 0, 'rex.b' : False, 'displacement' : 0,   'type' : 'r32',},  'registers' : [ 'eax', 'ecx', 'edx', 'ebx',  None,  None, 'esi', 'edi' ] },
		{ 'attrs' : {'mod' : 0, 'rex.b' : False, 'displacement' : 32,  'type' : 'r32',},  'registers' : [  None,  None,  None,  None,  None, 'eip',  None,  None ] },
		
		# r12, r13 dostopna zgolj z SIB
		{ 'attrs' : {'mod' : 0, 'rex.b' : True,  'displacement' : 0,   'type' : 'r64',},  'registers' : [ 'r8',  'r9',  'r10',  'r11',   None,  None, 'r14',  'rdi'  ] },
		{ 'attrs' : {'mod' : 0, 'rex.b' : True,  'displacement' : 32,  'type' : 'r64',},  'registers' : [  None,  None,  None,   None,   None, 'rip',  None,   None  ] },
		{ 'attrs' : {'mod' : 0, 'rex.b' : True,  'displacement' : 0,   'type' : 'r32',},  'registers' : [ 'r8d', 'r9d', 'r10d', 'r11d',  None,  None, 'r14d', 'r15d' ] },
		{ 'attrs' : {'mod' : 0, 'rex.b' : True,  'displacement' : 32,  'type' : 'r32',},  'registers' : [  None,  None,  None,   None,   None, 'eip',  None,   None  ] } ,

		{ 'attrs' : {'mod' : 1, 'rex.b' : False, 'displacement' : 8,   'type' : 'r64',},  'registers' : [ 'rax', 'rcx', 'rdx', 'rbx',   None, 'rbp', 'rsi', 'rdi'  ] },
		{ 'attrs' : {'mod' : 1, 'rex.b' : False, 'displacement' : 8,   'type' : 'r32',},  'registers' : [ 'eax', 'ecx', 'edx', 'ebx',   None, 'ebp', 'esi', 'edi'  ] },
		{ 'attrs' : {'mod' : 1, 'rex.b' : True,  'displacement' : 8,   'type' : 'r64',},  'registers' : [ 'r8',  'r9',  'r10',  'r11',  None, 'rbp', 'r14', 'rdi'  ] },
		{ 'attrs' : {'mod' : 1, 'rex.b' : True,  'displacement' : 8,   'type' : 'r32',},  'registers' : [ 'r8d', 'r9d', 'r10d', 'r11d', None, 'ebp', 'r14d','r15d' ] },

		{ 'attrs' : {'mod' : 2, 'rex.b' : False, 'displacement' : 32,  'type' : 'r64',},  'registers' : [ 'rax', 'rcx', 'rdx', 'rbx',   None, 'rbp', 'rsi', 'rdi'  ] },
		{ 'attrs' : {'mod' : 2, 'rex.b' : False, 'displacement' : 32,  'type' : 'r32',},  'registers' : [ 'eax', 'ecx', 'edx', 'ebx',   None, 'ebp', 'esi', 'edi'  ] },
		{ 'attrs' : {'mod' : 2, 'rex.b' : True,  'displacement' : 32,  'type' : 'r64',},  'registers' : [ 'r8',  'r9',  'r10',  'r11',  None, 'rbp', 'r14', 'rdi'  ] },
		{ 'attrs' : {'mod' : 2, 'rex.b' : True,  'displacement' : 32,  'type' : 'r32',},  'registers' : [ 'r8d', 'r9d', 'r10d', 'r11d', None, 'ebp', 'r14d','r15d' ] },

		{ 'attrs' : {'mod' : 3, 'rex.b' : False, 'displacement' : None,   'type' : 'r8',  }, 'registers' :  ['al', 'cl', 'dl', 'bl', 'spl', 'bpl', 'sil', 'dil']},
		{ 'attrs' : {'mod' : 3, 'rex.b' : True,  'displacement' : None,   'type' : 'r8',  }, 'registers' :  ['r8b', 'r9b', 'r10b', 'r11b', 'r12b', 'r13b', 'r14b', 'r15b'] },
		{ 'attrs' : {'mod' : 3, 'rex.b' : False, 'displacement' : None,   'type' : 'r16', }, 'registers' :  ['ax', 'cx', 'dx', 'bx', 'sp', 'bp', 'si', 'di', ]},
		{ 'attrs' : {'mod' : 3, 'rex.b' : True,  'displacement' : None,   'type' : 'r16', }, 'registers' :  ['r8w', 'r9w', 'r10w', 'r11w', 'r12w', 'r13w', 'r14w', 'r15w'] },
		{ 'attrs' : {'mod' : 3, 'rex.b' : False, 'displacement' : None,   'type' : 'r32', }, 'registers' :  ['eax', 'ecx', 'edx', 'ebx', 'esp', 'ebp', 'esi', 'edi', ]},
		{ 'attrs' : {'mod' : 3, 'rex.b' : True,  'displacement' : None,   'type' : 'r32', }, 'registers' :  ['r8d', 'r9d', 'r10d', 'r11d', 'r12d', 'r13d', 'r14d', 'r15d'] },
		{ 'attrs' : {'mod' : 3, 'rex.b' : False, 'displacement' : None,   'type' : 'r64', }, 'registers' :  ['rax', 'rcx', 'rdx', 'rbx', 'rsp', 'rbp', 'rsi', 'rdi', ]},
		{ 'attrs' : {'mod' : 3, 'rex.b' : True,  'displacement' : None,   'type' : 'r64', }, 'registers' :  ['r8', 'r9', 'r10', 'r11', 'r12', 'r13', 'r14', 'r15'] },
		{ 'attrs' : {'mod' : 3, 'rex.b' : False, 'displacement' : None,   'type' : 'mm',  }, 'registers' :  ['mm0', 'mm1', 'mm2', 'mm3', 'mm4', 'mm5', 'mm6', 'mm7', ]},
		{ 'attrs' : {'mod' : 3, 'rex.b' : True,  'displacement' : None,   'type' : 'mm',  }, 'registers' :  ['mm0', 'mm1', 'mm2', 'mm3', 'mm4', 'mm5', 'mm6', 'mm7'] },
		{ 'attrs' : {'mod' : 3, 'rex.b' : False, 'displacement' : None,   'type' : 'xmm', }, 'registers' :  ['xmm0', 'xmm1', 'xmm2', 'xmm3', 'xmm4', 'xmm5', 'xmm6', 'xmm7', ]},
		{ 'attrs' : {'mod' : 3, 'rex.b' : True,  'displacement' : None,   'type' : 'xmm', }, 'registers' :  ['xmm8', 'xmm9', 'xmm10', 'xmm11', 'xmm12', 'xmm13', 'xmm14', 'xmm15'] }	
		# Dodaj SIB
	]

	src_matches = []

	for tentative_match in source_registers:
		if (tentative_match['attrs']['displacement'] != displacement): 
		   continue
		for index, reg in enumerate(tentative_match['registers']):
			if src_register == reg:
				src_matches.append( (tentative_match['attrs'], index) )

	dst_matches = []

	for tentative_match in destination_registers:
		for index, reg in enumerate(tentative_match['registers']):
			if dst_register == reg:
				dst_matches.append( (tentative_match['attrs'], index) )


	for (src, reg), (dst, rm) in product(src_matches, dst_matches):
		
		modrm = (src['mod'] << 6) | ( rm << 3) | reg
		
		yield { 
			'rex.r': dst['rex.r'], 'rex.b': src['rex.b'], 
			'dst_type' : dst['type'], 'src_type' : src['type'], 
			'mod' : src['mod'],
			'modrm' : '%02x' % modrm,

		}

for match in modrm_encode('esp', 'ebx'):
	print(match)

for match in sib_encode('rdi', 4, 'r15'):
	print(match)

for match in sib_encode('rdi', 8, 'r15'):
	print(match)	