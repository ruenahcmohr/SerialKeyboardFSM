all:
	gcc *.c -o main
	./main

clean:
	rm main output.bin

