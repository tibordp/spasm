void executable_test(ostream& bytes, ostream& instructions)
{
	// Just to suppress the warning message
	bytes.flush();
	instructions.flush();

	instruction code;
	code.push_value<int64_t>(code.end(), -1337);	
	code.mov(R::rax, {R::rip, -15});
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