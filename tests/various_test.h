void various_test(ostream& bytes, ostream& instructions)
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

	#define VARIOUS1(instruction) \
	for (auto i : cpu_registers::all()) \
	{\
		if (X.instruction(i)) \
		{\
			instructions << #instruction << " " << i.name << endl;\
			print_bytes(X);\
		}\
	}	

	#define VARIOUS2(instruction) \
	every_sib([&] (sib_specifier sib) \
	{\
		if (X.instruction<1>(sib)) \
		{\
			instructions << #instruction << " BYTE PTR " << sib.to_string()  << endl;\
			print_bytes(X);\
		}\
		if (X.instruction<2>(sib)) \
		{\
			instructions << #instruction << " WORD PTR " << sib.to_string() <<  endl;\
			print_bytes(X);\
		}\
		if (X.instruction<4>(sib)) \
		{\
			instructions << #instruction << " DWORD PTR " << sib.to_string() <<  endl;\
			print_bytes(X);\
		}\
		if (X.instruction<8>(sib)) \
		{\
			instructions << #instruction << " QWORD PTR " << sib.to_string() <<  endl;\
			print_bytes(X);\
		}		\
	});	

	VARIOUS1(pop) VARIOUS2(pop)
	VARIOUS1(push) VARIOUS2(push)
	VARIOUS1(inc) VARIOUS2(inc)
	VARIOUS1(dec) VARIOUS2(dec)

	// -------------------------------------

	for (auto i : cpu_registers::all())
	{
		if (X.jmp(i)) 
		{
			instructions << "jmp" << " " << i.name << endl;
			print_bytes(X);
		}
	}

	every_sib([&] (sib_specifier sib) 
	{
		if (X.jmp(sib)) 
		{
			instructions <<  "jmp " << sib.to_string()  << endl;
			print_bytes(X);
		}
	});

	auto test = [&] (int32_t value) {
		if (X.jmp(value))
		{
			instructions << "jmp $" << dec << (int32_t)(value + X.size()) << endl;
			print_bytes(X);
		}
	};

	test(-15);
	test(-15000);

	// -------------------------------------

	for (auto i : cpu_registers::all())
	{
		if (X.call(i)) 
		{
			instructions << "call" << " " << i.name << endl;
			print_bytes(X);
		}
	}

	every_sib([&] (sib_specifier sib) 
	{
		if (X.call(sib)) 
		{
			instructions <<  "call " << sib.to_string()  << endl;
			print_bytes(X);
		}
	});

	auto test2 = [&] (int32_t value) {
		if (X.call(value))
		{
			instructions << "call $" << dec << (int32_t)(value + X.size()) << endl;
			print_bytes(X);
		}
	};

	test2(-15);
	test2(-15000);	


	int8_t x1 = 0x15;
	int16_t x2 = 0x1515;
	int32_t x3 = 0x15151515;
	int64_t x4 = 0x1515151515151515;

	if (X.push<1>(x1)) 
	{
		instructions << "push 0x" << hex << (int)x1  << endl;
		print_bytes(X);
	}

	/* GAS seems to prefer 32-bit immediates even when it can fit into 16 bits... */
	if (X.push<2>(x2)) 
	{
		instructions << "pushw 0x" << hex << x2 <<  endl;
		print_bytes(X);
	}
	if (X.push<4>(x3)) 
	{
		instructions << "push 0x" << hex << x3 <<  endl;
		print_bytes(X);
	}
	if (X.push<8>(x4)) 
	{
		instructions << "push 0x" << hex << x4 <<  endl;
		print_bytes(X);
	}	

	every_sib_reg([&] (cpu_register i, sib_specifier sib) 
	{
		if (X.lea(sib, i)) 
		{
			instructions << "lea " << i.name << ", " << sib.to_string() <<  endl;
			print_bytes(X);
		}		


	});


}