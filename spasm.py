import xml.etree.ElementTree as ET
desc1 = {
'A' : '	Direct address. The instruction has no ModR/M byte; the address of the operand is encoded in the instruction; no base register, index register, or scaling factor can be applied (for example, far JMP (EA)).',
'BA' : '	Memory addressed by DS:EAX, or by rAX in 64-bit mode (only 0F01C8 MONITOR).',
'BB' : '	Memory addressed by DS:eBX+AL, or by rBX+AL in 64-bit mode (only XLAT). (This code changed from single B in revision 1.00)',
'BD' : '	Memory addressed by DS:eDI or by RDI (only 0FF7 MASKMOVQ and 660FF7 MASKMOVDQU) (This code changed from YD (introduced in 1.00) in revision 1.02)',
'C' : '	The reg field of the ModR/M byte selects a control register (only MOV (0F20, 0F22)).',
'D' : '	The reg field of the ModR/M byte selects a debug register (only MOV (0F21, 0F23)).',
'E' : '	A ModR/M byte follows the opcode and specifies the operand. The operand is either a general-purpose register or a memory address. If it is a memory address, the address is computed from a segment register and any of the following values: a base register, an index register, a scaling factor, or a displacement.',
'ES' : '(Implies original E). A ModR/M byte follows the opcode and specifies the operand. The operand is either a x87 FPU stack register or a memory address. If it is a memory address, the address is computed from a segment register and any of the following values: a base register, an index register, a scaling factor, or a displacement.',
'EST' : '(Implies original E). A ModR/M byte follows the opcode and specifies the x87 FPU stack register.',
'F' : '	rFLAGS register.',
'G' : '	The reg field of the ModR/M byte selects a general register (for example, AX (000)).',
'H' : '	The r/m field of the ModR/M byte always selects a general register, regardless of the mod field (for example, MOV (0F20)).',
'I' : '	Immediate data. The operand value is encoded in subsequent bytes of the instruction.',
'J' : '	The instruction contains a relative offset to be added to the instruction pointer register (for example, JMP (E9), LOOP)).',
'M' : '	The ModR/M byte may refer only to memory: mod != 11bin (BOUND, LEA, CALLF, JMPF, LES, LDS, LSS, LFS, LGS, CMPXCHG8B, CMPXCHG16B, F20FF0 LDDQU).',
'N' : '	The R/M field of the ModR/M byte selects a packed quadword MMX technology register.',
'O' : '	The instruction has no ModR/M byte; the offset of the operand is coded as a word, double word or quad word (depending on address size attribute) in the instruction. No base register, index register, or scaling factor can be applied (only MOV  (A0, A1, A2, A3)).',
'P' : '	The reg field of the ModR/M byte selects a packed quadword MMX technology register.',
'Q' : '	A ModR/M byte follows the opcode and specifies the operand. The operand is either an MMX technology register or a memory address. If it is a memory address, the address is computed from a segment register and any of the following values: a base register, an index register, a scaling factor, and a displacement.',
'R' : '	The mod field of the ModR/M byte may refer only to a general register (only MOV (0F20-0F24, 0F26)).',
'S' : '	The reg field of the ModR/M byte selects a segment register (only MOV (8C, 8E)).',
'S2' : 'The two bits at bit index three of the opcode byte selects one of original four segment registers (for example, PUSH ES).',
'S30' : 'The three least-significant bits of the opcode byte selects segment register SS, FS, or GS (for example, LSS).',
'S33' : 'The three bits at bit index three of the opcode byte selects segment register FS or GS (for example, PUSH FS).',
'SC' : '	Stack operand, used by instructions which either push an operand to the stack or pop an operand from the stack. Pop-like instructions are, for example, POP, RET, IRET, LEAVE. Push-like are, for example, PUSH, CALL, INT. No Operand type is provided along with this method because it depends on source/destination operand(s).',
'T' : '	The reg field of the ModR/M byte selects a test register (only MOV (0F24, 0F26)).',
'U' : '	The R/M field of the ModR/M byte selects a 128-bit XMM register.',
'V' : '	The reg field of the ModR/M byte selects a 128-bit XMM register.',
'W' : '	A ModR/M byte follows the opcode and specifies the operand. The operand is either a 128-bit XMM register or a memory address. If it is a memory address, the address is computed from a segment register and any of the following values: a base register, an index register, a scaling factor, and a displacement',
'X' : '	Memory addressed by the DS:eSI or by RSI (only MOVS, CMPS, OUTS, and LODS). In 64-bit mode, only 64-bit (RSI) and 32-bit (ESI) address sizes are supported. In non-64-bit modes, only 32-bit (ESI) and 16-bit (SI) address sizes are supported.',
'Y' : '	Memory addressed by the ES:eDI or by RDI (only MOVS, CMPS, INS, STOS, and SCAS). In 64-bit mode, only 64-bit (RDI) and 32-bit (EDI) address sizes are supported. In non-64-bit modes, only 32-bit (EDI) and 16-bit (DI) address sizes are supported. The implicit ES segment register cannot be overriden by a segment prefix.',
'Z' : '	The instruction has no ModR/M byte; the three least-significant bits of the opcode byte selects a general-purpose register',
}

desc2 = {
'a' :	'Two one-word operands in memory or two double-word operands in memory, depending on operand-size attribute (only BOUND).',
'b' :	'Byte, regardless of operand-size attribute.',
'bcd' :	'Packed-BCD. Only x87 FPU instructions (for example, FBLD).',
'bs' :'; simplified bsq	Byte, sign-extended to the size of the destination operand.',
'bsq' :'; replaced by bs	(Byte, sign-extended to 64 bits.)',
'bss' :	'Byte, sign-extended to the size of the stack pointer (for example, PUSH (6A)).',
'c' :	'Byte or word, depending on operand-size attribute. (unused even by Intel?)',
'd' :	'Doubleword, regardless of operand-size attribute.',
'da' : 'Doubleword, according to address-size attribute (only JECXZ instruction).',
'di' :	'Doubleword-integer. Only x87 FPU instructions (for example, FIADD).',
'do' : 'Doubleword, according to current operand size (e. g., MOVSD instruction).',
'dq' :	'Double-quadword, regardless of operand-size attribute (for example, CMPXCHG16B).',
'dqa' : 'Doubleword or quadword, according to address-size attribute (only REP and LOOP families).',
'dqp' :'; combines d and qp	Doubleword, or quadword, promoted by REX.W in 64-bit mode (for example, MOVSXD).',
'dr' :	'Double-real. Only x87 FPU instructions (for example, FADD).',
'ds' :	'Doubleword, sign-extended to 64 bits (for example, CALL (E8).',
'e' :	'x87 FPU environment (for example, FSTENV).',
'er' :	'Extended-real. Only x87 FPU instructions (for example, FLD).',
'p' :	'32-bit or 48-bit pointer, depending on operand-size attribute (for example, CALLF (9A).',
'pd' :	'128-bit packed double-precision floating-point data.',
'pi' :	'Quadword MMX technology data.',
'ps' :	'128-bit packed single-precision floating-point data.',
'psq' :	'64-bit packed single-precision floating-point data.',
'pt' :'; replaced by ptp	(80-bit far pointer.)',
'ptp' :	'32-bit or 48-bit pointer, depending on operand-size attribute, or 80-bit far pointer, promoted by REX.W in 64-bit mode (for example, CALLF (FF /3)).',
'q' :	'Quadword, regardless of operand-size attribute (for example, CALL (FF /2)).',
'qa' : 'Quadword, according to address-size attribute (only JRCXZ instruction).',
'qi' :	'Qword-integer. Only x87 FPU instructions (for example, FILD).',
'qp' :	'Quadword, promoted by REX.W (for example, IRETQ).',
'qs' : 'Quadword, according to current stack size (only PUSHFQ and POPFQ instructions). }',
's' : 'to	6-byte pseudo-descriptor, or 10-byte pseudo-descriptor in 64-bit mode (for example, SGDT).',
'sd' :	'Scalar element of a 128-bit packed double-precision floating data.',
'si' :	'Doubleword integer register (e. g., eax). (unused even by Intel?)',
'sr' :	'Single-real. Only x87 FPU instructions (for example, FADD).',
'ss' :	'Scalar element of a 128-bit packed single-precision floating data.',
'st' :	'x87 FPU state (for example, FSAVE).',
'stx' :	'x87 FPU and SIMD state (FXSAVE and FXRSTOR).',
't' :'; replaced by ptp	10-byte far pointer.',
'v' :	'Word or doubleword, depending on operand-size attribute (for example, INC (40), PUSH (50)).',
'va' : 'Word or doubleword, according to address-size attribute (only REP and LOOP families).',
'vds' :'combines v and ds	Word or doubleword, depending on operand-size attribute, or doubleword, sign-extended to 64 bits for 64-bit operand size.',
'vq' :	'Quadword (default) or word if operand-size prefix is used (for example, PUSH (50)).',
'vqp' :'combines v and qp	Word or doubleword, depending on operand-size attribute, or quadword, promoted by REX.W in 64-bit mode.',
'vs' :	'Word or doubleword sign extended to the size of the stack pointer (for example, PUSH (68)).',
'w' :	'Word, regardless of operand-size attribute (for example, ENTER).',
'wa' : 'Word, according to address-size attribute (only JCXZ instruction).',
'wi' :	'Word-integer. Only x87 FPU instructions (for example, FIADD).',
'wo' : 'Word, according to current operand size (e. g., MOVSW instruction).',
'ws' : 'Word, according to current stack size (only PUSHF and POPF instructions in 64-bit mode).' }

def textify(elem):
	if elem is None:
		return None
	return elem.text

class Base(object):
	"""docstring for Operand"""
	def __init__(self, data):
		self.data = data

	def __bool__(self):
		return self.data is not None		

class Address(Base):
	@property
	def description(self):
		return desc1[self.data]

class Type(Base):
	@property
	def description(self):
		return desc2[self.data]

class Operand(Base):
	@property	
	def address(self):
		if self.data.find('a') is not None:
			key = self.data.find('a').text
		else:
			key = self.data.get('address')		
		if key is None:
			return None
		
		return Address(key)

	@property
	def type(self):
		if self.data.find('t') is not None:
			key = self.data.find('t').text
		else:
			key = self.data.get('type')			
		if key is None:
			return None
		
		return Type(key)

	def satisfies(self, value):
		if not self:
			return False
		if not self.address:
			return False
		return self.address.data == value

class Syntax(Base):
	@property
	def mnemonic(self):
		return textify(self.data.find('mnem'))		

	@property
	def operands(self):
		for child in self.data:
			if child.tag in { 'src', 'dst' }:
				yield Operand(child)
		

class Entry(Base):
	@property
	def syntax(self):
		return Syntax(self.data.find('syntax'))

	@property
	def description(self):
		return self.data.find('note').find('brief').text	

class Opcode(Base):
	@property
	def entries(self):
		for entry in op.findall('entry'):
			yield Entry(entry)
	
	@property
	def value(self):
		return self.data.get('value')

	

with open('x86reference.xml','rb') as file:
	ref = file.read().decode('utf-8')

root = ET.fromstring(ref)
one_byte = root.find('one-byte')

for op in one_byte.findall('pri_opcd'):
	Op = Opcode(op)
	for entry in Op.entries:
		if entry and entry.syntax and entry.syntax.mnemonic is not None:
			syn = entry.syntax

			print ('%s (%s)' % (syn.mnemonic, Op.value) )

			for i, oper in enumerate(syn.operands):
				(a,t) = (oper.address, oper.type)
				print('  Opcode %d' % i)
				if a:
					print ('\tAddress: ' + a.description)
				if t:
					print ('\tType:    ' + t.description)
				print('')
		
			print('')


