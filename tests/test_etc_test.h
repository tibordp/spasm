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
		if (X.T_COMMON(i)) 
		{
			instructions << name() << " " << i.name << endl;
			print_bytes(X);
		}	
	

	// T_COMMON [4*rax + ecx + 0x1337], 0x....

	for (auto j : cpu_registers::all())
	for (auto k : cpu_registers::all())
	for (int multiplier = 1; multiplier <= 8; multiplier *= 2)
	for (int32_t displacement : { 0, 0x15, 0x1515 })
	{
		sib_specifier sib({ multiplier, j, k, displacement });

		if (X.T_COMMON<1>(sib)) 
		{
			instructions << name() << " BYTE PTR " << sib.to_string() << endl;
			print_bytes(X);
		}
		if (X.T_COMMON<2>(sib)) 
		{
			instructions << name() << " WORD PTR " << sib.to_string() <<  endl;
			print_bytes(X);
		}
		if (X.T_COMMON<4>(sib)) 
		{
			instructions << name() << " DWORD PTR " << sib.to_string() <<  endl;
			print_bytes(X);
		}
		if (X.T_COMMON<8>(sib)) 
		{
			instructions << name() << " QWORD PTR " << sib.to_string() <<  endl;
			print_bytes(X);
		}		
	}
}


#ifndef TEST_DEFINED
void test_test(ostream& bytes, ostream& instructions)
{
	auto name = [] {
		stringstream ss;
		for (char i : string(STRINGIFY(test)))
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

	// test rax, rcx

	instruction X;
	
	// T_COMMON rax, 0x...
	
	for (auto i : cpu_registers::all())
	{
		int8_t x1 = 0x15; 	
		int16_t x2 = 0x1515; 	
		int32_t x3 = 0x15151515; 		

		if (X.test<1>(i, x1)) 
		{
			instructions << name() << " " << i.name << ", 0x" <<  std::hex  << (int)x1 << endl;
			print_bytes(X);
		}
		if (X.test<2>(i, x2)) 
		{
			instructions << name() << " " << i.name << ", 0x" <<  std::hex  << x2 << endl;
			print_bytes(X);
		}
		if (X.test<4>(i, x3)) 
		{
			instructions << name() << " " << i.name << ", 0x" <<  std::hex  << x3 << endl;
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

		if (X.test<1>(sib, x1)) 
		{
			instructions << name() << " BYTE PTR " << sib.to_string() << ", 0x" <<  std::hex  << (int)x1 << endl;
			print_bytes(X);
		}
		if (X.test<2>(sib, x2)) 
		{
			instructions << name() << " WORD PTR " << sib.to_string() << ", 0x" <<  std::hex  << x2 << endl;
			print_bytes(X);
		}
		if (X.test<4>(sib, x3)) 
		{
			instructions << name() << " DWORD PTR " << sib.to_string() << ", 0x" <<  std::hex  << x3 << endl;
			print_bytes(X);
		}
	}
}
#endif

#define TEST_DEFINED