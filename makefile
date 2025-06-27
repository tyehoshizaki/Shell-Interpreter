make:
	gcc -o ssi ssi.c helpers/pipe.c helpers/shell_commands.c -lreadline

clean:
	rm ssi