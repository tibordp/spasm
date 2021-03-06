void mov_test(ostream& bytes, ostream& instructions)
{
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

	instruction X;
	
	for (auto i : cpu_registers::all())
	for (auto j : cpu_registers::all())
		if (X.mov(i, j)) 
		{
			instructions << "mov " << i.name << ", " << j.name << endl;
			print_bytes(X);
		}

	for (auto i : cpu_registers::all())
	{
		int8_t x1 = 0x15; 	
		int16_t x2 = 0x1515; 	
		int32_t x3 = 0x15151515; 	
		int64_t x4 = 0x1515151515151515; 	

		if (X.mov<1>(i, x1)) 
		{
			instructions << "mov " << i.name << ", 0x" <<  std::hex  << (int)x1 << endl;
			print_bytes(X);
		}
		if (X.mov<2>(i, x2)) 
		{
			instructions << "mov " << i.name << ", 0x" <<  std::hex  << x2 << endl;
			print_bytes(X);
		}
		if (X.mov<4>(i, x3)) 
		{
			instructions << "mov " << i.name << ", 0x" <<  std::hex  << x3 << endl;
			print_bytes(X);
		}
		if (X.mov<8>(i, x4)) 
		{
			instructions << "mov " << i.name << ", 0x" <<  std::hex  << x4 << endl;	
			print_bytes(X);
		}
	}		

	// mov rax, [4*rax + ecx + 0x1337]
	// mov [4*rax + ecx + 0x1337], rax

	every_sib_reg([&] (cpu_register i, sib_specifier sib) 
	{
		if (X.mov(sib, i)) 
		{
			instructions << "mov " << sib.to_string() << ", " << i.name << endl;
			print_bytes(X);
		}
		if (X.mov(i, sib)) 
		{
			instructions << "mov " << i.name << ", " << sib.to_string() << endl;
			print_bytes(X);
		}
	});
	

	// mov [4*rax + ecx + 0x1337], 0x....

	every_sib([&] (sib_specifier sib) 
	{
		int8_t x1 = 0x15;
		int16_t x2 = 0x1515;
		int32_t x3 = 0x15151515;
		int64_t x4 = 0x1515151515151515;

		if (X.mov<1>(sib, x1)) 
		{
			instructions << "mov BYTE PTR " << sib.to_string() << ", 0x" <<  std::hex  << (int)x1 << endl;
			print_bytes(X);
		}
		if (X.mov<2>(sib, x2)) 
		{
			instructions << "mov WORD PTR " << sib.to_string() << ", 0x" <<  std::hex  << x2 << endl;
			print_bytes(X);
		}
		if (X.mov<4>(sib, x3)) 
		{
			instructions << "mov DWORD PTR " << sib.to_string() << ", 0x" <<  std::hex  << x3 << endl;
			print_bytes(X);
		}
		if (X.mov<8>(sib, x4)) 
		{
			instructions << "mov QWORD PTR " << sib.to_string() << ", 0x" <<  std::hex  << x4 << endl;
			print_bytes(X);
		}		
	});
}

void xchg_test(ostream& bytes, ostream& instructions)
{
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

	instruction X;
	
	for (auto i : cpu_registers::all())
	for (auto j : cpu_registers::all())
		if (X.xchg(i, j)) 
		{
			instructions << "xchg " << i.name << ", " << j.name << endl;
			print_bytes(X);
		}


	// xchg rax, [4*rax + ecx + 0x1337]
	// xchg [4*rax + ecx + 0x1337], rax

	every_sib_reg([&] (cpu_register i, sib_specifier sib) 
	{
		if (X.xchg(sib, i)) 
		{
			instructions << "xchg " << sib.to_string() << ", " << i.name << endl;
			print_bytes(X);
		}
		if (X.xchg(i, sib)) 
		{
			instructions << "xchg " << i.name << ", " << sib.to_string() << endl;
			print_bytes(X);
		}
	});

}