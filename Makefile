output=iscsi

all:
	gcc $(shell find . -name '*.c') -I . -o $(output)

clean:
	rm ./$(output)
