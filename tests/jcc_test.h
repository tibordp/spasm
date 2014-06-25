void jcc_test(ostream& bytes, ostream& instructions)
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
	}; instruction X;

	#define JCC_CASE(op) { &instruction::op , #op }

	initializer_list<pair<decltype(&instruction::jecxz), string>> cases = {
		JCC_CASE(ja),
		JCC_CASE(jae),
		JCC_CASE(jb),
		JCC_CASE(jbe),
		JCC_CASE(jc),
		JCC_CASE(je),
		JCC_CASE(jg),
		JCC_CASE(jge),
		JCC_CASE(jl),
		JCC_CASE(jle),
		JCC_CASE(jna),
		JCC_CASE(jnae),
		JCC_CASE(jnb),
		JCC_CASE(jnbe),
		JCC_CASE(jnc),
		JCC_CASE(jne),
		JCC_CASE(jng),
		JCC_CASE(jnge),
		JCC_CASE(jnl),
		JCC_CASE(jnle),
		JCC_CASE(jno),
		JCC_CASE(jnp),
		JCC_CASE(jns),
		JCC_CASE(jnz),
		JCC_CASE(jo),
		JCC_CASE(jp),
		JCC_CASE(jpe),
		JCC_CASE(jpo),
		JCC_CASE(js),
		JCC_CASE(jz),
		JCC_CASE(jecxz),
		JCC_CASE(jrcxz),
	};

	/* ATTENTION - a different convention for relative jumps is chosen than gas uses
	   hence the value + X.size(), the instruction itself is counted in the offset. */

	auto test = [&] (int32_t value) {
		for (auto i: cases)
		{
			if ((X.*(i.first))(value))
			{
				instructions << i.second << " $" << dec << (int32_t)(value + X.size()) << endl;
				print_bytes(X);
			}
		}

	};

	test(-15);
	test(-15000);
}