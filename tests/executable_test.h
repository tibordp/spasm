void executable_test(ofstream& bytes, ofstream& instructions)
{
	// Just to suppress the warning message
	bytes.flush();
	instructions.flush();

	instruction code;
	code.push_value<int64_t>(code.end(), -1337);	

	auto ins = mov(R::rax, {R::rip, -15});

	code.insert(code.end(), ins.begin(), ins.end());
	code.push_back(0xc3);

	x_function<int64_t(void)> fun(&code[0], code.size());
	fun.set_entry(8);

	if (fun() != -1337)
		throw runtime_error("Incorrect value.");

	auto fun2 = fun;

	if (fun2() != -1337)
		throw runtime_error("Incorrect value.");

	auto fun3 = std::move(fun2);

	if (fun3() != -1337)
		throw runtime_error("Incorrect value.");

	if (fun() != -1337)
		throw runtime_error("Incorrect value.");
}