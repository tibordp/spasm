/*
	Exhaustively tests every possible combination of registers (direct, indirect), SIB combinations,
	displacements and immediate operand sizes for `mov` instruction.
*/

void mov_test(ofstream& bytes, ofstream& instructions)
{
	auto print_X = [&] (const instruction& X) {
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
	};

	// mov rax, rcx

	for (auto i : cpu_registers::all())
	for (auto j : cpu_registers::all())
	{
		auto X = mov(i, j);
		if (!X.empty())
		{
			instructions << "mov " << i.name << ", " << j.name << endl;
			print_X(X);
		}
	}

	// mov rax, 0x...
	
	for (auto i : cpu_registers::all())
	{

		int8_t x1 = 0x15; auto X1 = movb(i, x1);
		int16_t x2 = 0x1515; auto X2 = movw(i, x2);
		int32_t x3 = 0x15151515; auto X3 = movl(i, x3);
		int64_t x4 = 0x1515151515151515; auto X4 = movq(i, x4);

		if (!X1.empty())
		{
			instructions << "mov " << i.name << ", 0x" <<  std::hex  << (int)x1 << endl;
			print_X(X1);
		}

		if (!X2.empty())
		{
			instructions << "mov " << i.name << ", 0x" <<  std::hex  << x2 << endl;
			print_X(X2);
		}

		if (!X3.empty())
		{
			instructions << "mov " << i.name << ", 0x" <<  std::hex  << x3 << endl;
			print_X(X3);
		}

		if (!X4.empty())
		{
			instructions << "mov " << i.name << ", 0x" <<  std::hex  << x4 << endl;
			print_X(X4);
		}		
	}		

	// mov rax, [4*rax + ecx + 0x1337]
	// mov [4*rax + ecx + 0x1337], rax

	for (auto i : cpu_registers::all())
	for (auto j : cpu_registers::all())
	for (auto k : cpu_registers::all())
	for (int multiplier = 1; multiplier <= 8; multiplier *= 2)
	for (int32_t displacement : { 0, 0x15, 0x1515 })
	for (bool direction : { true, false })
	{
		sib_specifier sib({ multiplier, j, k, displacement });
		auto X = mov_indirect(sib, i, direction);
	
		if (!X.empty())
		{
			if (direction)
				instructions << "mov " << sib.to_string() << ", " << i.name << endl;
			else
				instructions << "mov " << i.name << ", " << sib.to_string() << endl;
			print_X(X);
		}
	}
	

	// mov [4*rax + ecx + 0x1337], 0x....

	for (auto j : cpu_registers::all())
	for (auto k : cpu_registers::all())
	for (int multiplier = 1; multiplier <= 8; multiplier *= 2)
	for (int32_t displacement : { 0, 0x15, 0x1515 })
	{
		sib_specifier sib({ multiplier, j, k, displacement });

		int8_t x1 = 0x15; auto X1 = movb(sib, x1);
		int16_t x2 = 0x1515; auto X2 = movw(sib, x2);
		int32_t x3 = 0x15151515; auto X3 = movl(sib, x3);

		if (!X1.empty())
		{
			instructions << "mov BYTE PTR " << sib.to_string() << ", 0x" <<  std::hex  << (int)x1 << endl;
			print_X(X1);
		}

		if (!X2.empty())
		{
			instructions << "mov WORD PTR " << sib.to_string() << ", 0x" <<  std::hex  << x2 << endl;
			print_X(X2);
		}

		if (!X3.empty())
		{
			instructions << "mov DWORD PTR " << sib.to_string() << ", 0x" <<  std::hex  << x3 << endl;
			print_X(X3);
		}
	}
}