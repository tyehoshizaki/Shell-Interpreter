make:
	gcc -o ssi ssi.c helpers/pipe.c helpers/shell_commands.c \
  -I/usr/local/opt/readline/include \
  -L/usr/local/opt/readline/lib \
  -lreadline

clean:
	rm ssi