#include <stdio.h>
#include <BearLibTerminal.h>


int main(int argc, char *argv[]) {
	printf("Running innfin\n");
	
	terminal_open();
	terminal_put(10, 20, 175);
	terminal_delay(1000);
	terminal_read();
	terminal_check(1);
	terminal_close();
	
	return 0;
}
