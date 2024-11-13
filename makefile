build_and_run:
	@lex -o lexer.cpp lexer.l
	@clang++-17 -g -O3 main.cpp lexer.cpp `llvm-config-17 --cxxflags --ldflags --system-libs --libs core` -o main -ll
	@./main

clean:
	rm lexer.cpp main