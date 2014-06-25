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

#include <thread>

x_function<void(int)> sloncki()
{
	static const char format_string[] = "%d malih slonckov se je pozibavalo\n" 
	                                    "na pajcevini tam po drevesom.\n"
	                                    "Ko so ugotovili, da stvar je zanimiva,\n"
	                                    "so poklicali se enega sloncka.\n\n";
	instruction code;

	const uint8_t frame_size = 0x28;
	
	// instruction does not own the labels!
	code_label print, format, start, loop;

	code.label(print);
	code.push_value(code.end(), &printf);

	code.label(format);
	code.push_value(code.end(), &format_string[0]);

	code.label(start);
		code.push(R::r12);
		code.push(R::r13);
	
		code.sub<1>(R::rsp, frame_size); // 0x20 for shadow space (req. per ABI), 0x8 for stack alignment
		
		code.mov(R::r13, R::rcx);        // Save the parameter to be reused
		code.xor_(R::r12, R::r12);       // Set r12 to 0

	code.label(loop);
		code.mov(R::rcx, { format });  // The first parameter is the format string	
		code.mov(R::rdx, R::r12);		 // Copy the first parameter to second parameter
		code.inc(R::rdx);				 // We start at 1.
		
		code.call({ print });
	
		code.inc(R::r12);
		code.cmp(R::r12, R::r13);
		code.jne(loop);
	
	code.add<1>(R::rsp, frame_size); // Restore the stack pointer
	code.pop(R::r13);
	code.pop(R::r12);
	
	code.ret();

	return code.compile<void(int)>(start);
}

void executable_test2(ostream& bytes, ostream& instructions)
{
	bytes.flush();
	instructions.flush();
	
	auto f = sloncki();
	f(100);
}