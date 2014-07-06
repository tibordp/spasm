# spasm

spasm is a x86-64 assembler / JIT support library. It enables you to write a machine code generator in C++ using assembly mnemonics for instructions. spasm does not parse the mnemonics from text but rather implements corresponding C++ functions. For example:

    spasm::instruction code;
    spasm::code_label puts_pointer, start;
    
    // 0x20 for shadow space (req. per ABI), 0x8 for stack alignment
    const uint8_t frame_size = 0x28;

    // We store the pointer to `puts` just above our code.
    code.label(puts_pointer);           
        code.push_value(code.end(), &puts);  
    
    code.label(start);
        code.mov({R::rsp, 0x8}, R::rcx);  // Store the first parameter to shadow space
        code.sub<1>(R::rsp, frame_size); 
        code.call({ puts_pointer });
        code.mov(R::rcx, {R::rsp, 0x8 + frame_size}); // Restore the first parameter
        code.call({ puts_pointer });
        code.add<1>(R::rsp, frame_size);  // Restore the stack pointer
        code.ret();
    
After the code as been constructed, it can be wrapped into a `std::function`-like wrapper and immediately executed:

    auto twice_puts = code.compile<void(const char*)>(start);
    twice_puts("Hello world!"); // Prints "Hello world!" twice.
    
### Dependencies


spasm has no dependencies apart from standard C++ libraries and appropriate platform-specific headers that contain `VirtualAlloc`, `mmap`, ...
