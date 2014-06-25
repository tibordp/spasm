void ATTACH_TEST(T_COMMON)(ostream& bytes, ostream& instructions)
{
	auto  name = [] {
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

	// mov rax, rcx

	instruction X;
	
	for (auto i : cpu_registers::all())
		if (X.T_COMMON(i)) 
		{
			instructions << name() << " " << i.name << endl;
			print_bytes(X);
		}

	for (auto i : cpu_registers::all())
		if (X.T_COMMON(i, R::cl)) 
		{
			instructions << name() << " " << i.name << ", cl" << endl;
			print_bytes(X);
		}

	uint8_t con = 0x15;

	for (auto i : cpu_registers::all())
		if (X.T_COMMON(i, con)) 
		{
			instructions << name() << " " << i.name << ", 0x" << hex << (int)con << endl;
			print_bytes(X);
		}		

	every_sib([&] (sib_specifier sib) 
	{
		if (X.T_COMMON<1>(sib)) 
		{
			instructions << name() << " BYTE PTR " << sib.to_string() << endl;
			print_bytes(X);
		}
		if (X.T_COMMON<2>(sib)) 
		{
			instructions << name() << " WORD PTR " << sib.to_string() << endl;
			print_bytes(X);
		}
		if (X.T_COMMON<4>(sib)) 
		{
			instructions << name() << " DWORD PTR " << sib.to_string() << endl;
			print_bytes(X);
		}
		if (X.T_COMMON<8>(sib)) 
		{
			instructions << name() << " QWORD PTR " << sib.to_string() << endl;
			print_bytes(X);
		}

		if (X.T_COMMON<1>(sib, R::cl)) 
		{
			instructions << name() << " BYTE PTR " << sib.to_string() << ",  cl" << endl;
			print_bytes(X);
		}
		if (X.T_COMMON<2>(sib, R::cl)) 
		{
			instructions << name() << " WORD PTR " << sib.to_string() << ",  cl" << endl;
			print_bytes(X);
		}
		if (X.T_COMMON<4>(sib, R::cl)) 
		{
			instructions << name() << " DWORD PTR " << sib.to_string() << ",  cl" << endl;
			print_bytes(X);
		}
		if (X.T_COMMON<8>(sib, R::cl)) 
		{
			instructions << name() << " QWORD PTR " << sib.to_string() << ",  cl" << endl;
			print_bytes(X);
		}		

		if (X.T_COMMON<1>(sib, con)) 
		{
			instructions << name() << " BYTE PTR " << sib.to_string() << ", 0x" <<  std::hex  << (int)con << endl;
			print_bytes(X);
		}
		if (X.T_COMMON<2>(sib, con)) 
		{
			instructions << name() << " WORD PTR " << sib.to_string() << ", 0x" <<  std::hex  << (int)con << endl;
			print_bytes(X);
		}
		if (X.T_COMMON<4>(sib, con)) 
		{
			instructions << name() << " DWORD PTR " << sib.to_string() << ", 0x" <<  std::hex  << (int)con << endl;
			print_bytes(X);
		}
		if (X.T_COMMON<8>(sib, con)) 
		{
			instructions << name() << " QWORD PTR " << sib.to_string() << ", 0x" <<  std::hex  << (int)con << endl;
			print_bytes(X);
		}
	});

}