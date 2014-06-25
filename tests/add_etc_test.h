/*
	Exhaustively tests every possible combination of registers (direct, indirect), SIB combinations,
	displacements and immediate operand sizes for `T_COMMON` instruction.
*/

#ifndef ATTACH_TEST
#define XATTACH_TEST(name) name##_test 
#define ATTACH_TEST(name) XATTACH_TEST(name) 
#define XSTRINGIFY(name) #name
#define STRINGIFY(name) XSTRINGIFY(name)
#endif

void ATTACH_TEST(T_COMMON)(ostream& bytes, ostream& instructions)
{
	auto name = [] {
		stringstream ss;
		for (char i : string(STRINGIFY(T_COMMON)))
			if (i != '_')
				ss << i;
		return ss.str();
	};

	auto print_bytes = [&] (instruction& X) {
		bool first = true;
		for (int i : X)
		{
			if (first)
				first = false;
			else
				bytes << " ";
			bytes << setfill('0') << setw(2) << hex << i;
		}
		bytes << endl;
		X.clear();
	};

	// T_COMMON rax, rcx

	instruction X;
	
	for (auto i : cpu_registers::all())
	for (auto j : cpu_registers::all())
		if (X.T_COMMON(i, j)) 
		{
			instructions << name() << " " << i.name << ", " << j.name << endl;
			print_bytes(X);
		}


	// T_COMMON rax, 0x...
	
	for (auto i : cpu_registers::all())
	{
		int8_t x1 = 0x15; 	
		int16_t x2 = 0x1515; 	
		int32_t x3 = 0x15151515; 		

		if (X.T_COMMON<1>(i, x1)) 
		{
			instructions << name() << " " << i.name << ", 0x" <<  std::hex  << (int)x1 << endl;
			print_bytes(X);
		}
		if (X.T_COMMON<2>(i, x2)) 
		{
			instructions << name() << " " << i.name << ", 0x" <<  std::hex  << x2 << endl;
			print_bytes(X);
		}
		if (X.T_COMMON<4>(i, x3)) 
		{
			instructions << name() << " " << i.name << ", 0x" <<  std::hex  << x3 << endl;
			print_bytes(X);
		}
	}		

	// T_COMMON rax, [4*rax + ecx + 0x1337]
	// T_COMMON [4*rax + ecx + 0x1337], rax

	for (auto i : cpu_registers::all())
	for (auto j : cpu_registers::all())
	for (auto k : cpu_registers::all())
	for (int multiplier = 1; multiplier <= 8; multiplier *= 2)
	for (int32_t displacement : { 0, 0x15, 0x1515 })
	{
		sib_specifier sib({ multiplier, j, k, displacement });
	
		if (X.T_COMMON(sib, i)) 
		{
			instructions << name() << " " << sib.to_string() << ", " << i.name << endl;
			print_bytes(X);
		}
		if (X.T_COMMON(i, sib)) 
		{
			instructions << name() << " " << i.name << ", " << sib.to_string() << endl;
			print_bytes(X);
		}

	}
	

	// T_COMMON [4*rax + ecx + 0x1337], 0x....

	for (auto j : cpu_registers::all())
	for (auto k : cpu_registers::all())
	for (int multiplier = 1; multiplier <= 8; multiplier *= 2)
	for (int32_t displacement : { 0, 0x15, 0x1515 })
	{
		sib_specifier sib({ multiplier, j, k, displacement });

		int8_t x1 = 0x15;
		int16_t x2 = 0x1515;
		int32_t x3 = 0x15151515;

		if (X.T_COMMON<1>(sib, x1)) 
		{
			instructions << name() << " BYTE PTR " << sib.to_string() << ", 0x" <<  std::hex  << (int)x1 << endl;
			print_bytes(X);
		}
		if (X.T_COMMON<2>(sib, x2)) 
		{
			instructions << name() << " WORD PTR " << sib.to_string() << ", 0x" <<  std::hex  << x2 << endl;
			print_bytes(X);
		}
		if (X.T_COMMON<4>(sib, x3)) 
		{
			instructions << name() << " DWORD PTR " << sib.to_string() << ", 0x" <<  std::hex  << x3 << endl;
			print_bytes(X);
		}
	}
}