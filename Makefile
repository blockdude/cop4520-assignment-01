all: main

run: all
	@exec ./main

main: main.cpp
	gcc main.cpp -lstdc++ -lm -o main

clean:
	rm main

.PHONY: clean run all
