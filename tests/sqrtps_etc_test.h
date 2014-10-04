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

	instruction X;
	
	for (auto i : cpu_registers::all())
	for (auto j : cpu_registers::all())
		if (X.T_COMMON(i, j)) 
		{
			instructions << name() << " " << i.name << ", " << j.name << endl;
			print_bytes(X);
		}

	for (auto i : cpu_registers::all())
	for (auto j : cpu_registers::all())
	for (auto k : cpu_registers::all())
	for (int multiplier = 1; multiplier <= 8; multiplier *= 2)
	for (int32_t displacement : { 0, 0x15, 0x1515 })
	{
		sib_specifier sib({ multiplier, j, k, displacement });

		if (X.T_COMMON(i, sib)) 
		{
			instructions << name() << " " << i.name << ", " << sib.to_string() << endl;
			print_bytes(X);
		}
	}
}