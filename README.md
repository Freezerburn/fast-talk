FastTalk
========

A (**REALLY early in development**) prototype scripting language written in C. For no other (main) reason than I enjoy
writing C in my spare time. (and because C++ is too complicated, Rust has gotten so big/complex, Zig isn't done yet,
and I like that writing C gives the most portability. Using an LLVM-based language means I can only target what LLVM
can target, while C runs on everything)

A list of inspirations for the language:

- Smalltalk: For the basic idea of using messages to structure code.
- Objective-C: For the optimized message-send code in the runtime. As well as all the cool things it allowed such as
  method swizzling, creating types at runtime, checking if an object supported handling a message, SEL/IMP/etc., and
  so on and so forth.
- Go: For (eventually) easy parallelism and green threading.
- Python: For how to structure the code for an interpreter to allow for things like polymorphism or just ease of
  creating new types that get added to the environment