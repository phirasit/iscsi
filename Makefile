c_option=-pthread -g
output=iscsi

all:
	gcc $(shell find . -name '*.c') -I . $(c_option) -o $(output)

clean:
	rm ./$(output)
