/**********************************************************************************************
* Programmer: Christian Webber
* Group #1, Seq #19
* Class: CptS 360, Spring 2014;
* Lab 2
* Created: February 4th, 2014
* Last Revised: February 4th, 2014
* File: main.c
*********************************************************************************************/

#include "common.h"

int savedPC, savedFP; // globals

int main(void)
{
	int input = 0;
	while (1)
	{
		printf("\nChoose a Part of the Lab to Run:\n 1- Part 1\n 2- Part 2\n 3- Quit\n");
		scanf(" %d", &input);

		if (input == 1)
			part1_main();
		else if (input == 2)
			part2_main();
		else if (input == 3)
			break;
	}

	return 0;
}

//////////// Part Main Functions ////////////
void part1_main(void)
{
	myprintf("Hello World!\n"); // good
	myprintf("Am I on a new line?\n"); // good
	myprintf("Int: %d (should be 3)\n", 3); // good
	myprintf("Negative Int: %d (should be -7)\n", -7); // good
	myprintf("String: %s (should be 'I'm Alive!')\n", "I'm Alive!"); // good
	myprintf("Hex: %x (should be 559E7C4)\n", 89778116); // good
	myprintf("Char: %c (should be 'z')\n", 'z'); // good
	myprintf("Full Check-\nInt: %d (5)\nNegative Int: %d (-121)\nString: %s ('The cake is a lie!')\nHex: %x (12FD100)\nChar: %c ('y')\n", 5, -121, "The cake is a lie!", 19910912, 'y');
}

void part2_main(void)
{
	int r, a, b, c;
	printf("enter main\n");
	a = 1; b = 2; c = 3;

	r = save();
	if (r == 0){
		A();
		printf("normal return\n");
	}
	else{
		printf("back to main via long jump r=%d\n", r);
		printf("a=%d b=%d c=%d\n", a, b, c);
	}
	printf("exit main\n");
}

//////////// Part 1 Functions ////////////
// myprintf: prints a passed string!
int myprintf(char *fmt, ...)
{
	if (strcmp(fmt, "") != 0)
	{
		int i = 0, *ebp = getebp(), *ip = ebp + 3;
		for (i = 0; fmt[i] != '\0'; i++)
		{
			if (fmt[i] == '%')
			{
				i++;
				if (fmt[i] == 'c')
					printc(*ip);
				else if (fmt[i] == 'd')
					printd(*ip);
				else if (fmt[i] == 'x')
					printx(*ip);
				else if (fmt[i] == 's')
					prints(*ip);
				else if (fmt[i] == '%')
					putchar('%');
				else
					return 0;
				ip++;
			}
			else if (fmt[i] == '/')
			{
				i++;
				if (fmt[i] == 'n')
				{
					putchar('\n');
					putchar('\r');
				}
				i++;
			}
			else
				putchar(fmt[i]);
		}
	}
	return 1;
}

//// Printing Helper Functions
// print an integer
void printd(int x)
{
	if (x == 0)
		putchar('0');
	else if (x < 0)
	{
		putchar('-');
		rpd(-x);
	}
	else
		rpd(x);
	putchar(' ');
}

// print a hex value
void printx(unsigned long x)
{
	if (x == 0)
		putchar('0');
	else
		rpx(x);
	putchar(' ');
}

// print a character
void printc(char c)
{
	putchar(c);
}

// print a string
void prints(char *s)
{
	int i = 0;
	for (i = 0; s[i] != '\0'; i++)
		putchar(s[i]);
	putchar(' ');
}
// prints an unsigned long (non-negative number)
void printu(unsigned long x)
{
	if (x == 0)
		putchar('0');
	else
		rpu(x);
	putchar(' ');
}

//// Printing Recursive Helper Functions
// recursive helper for printu
void rpd(int x)
{
	char c;
	if (x)
	{
		c = table[x % BASE];
		rpu(x / BASE);
		putchar(c);
	}
}

// recursive helper for printu
void rpx(unsigned long x)
{
	char c;
	if (x)
	{
		c = table[x % HEX];
		rpx(x / HEX);
		putchar(c);
	}
}

// recursive helper for printu
void rpu(unsigned long x)
{
	char c;
	if (x)
	{
		c = table[x % BASE];
		rpu(x / BASE);
		putchar(c);
	}
}

//////////// Part 2 Functions ////////////
int A(void)
{
	printf("enter A\n");
	B();
	printf("exit A\n");
}

int B(void)
{
	printf("enter B\n");
	C();
	printf("exit B\n");
}

int C(void)
{
	printf("enter C\n");
	printf("long jump? (y|n) ");

	if (getchar() == 'y')
		longjump(12345);

	printf("exit C\n");
}

int longjump(int v)
{
	int *ebp = getebp();

	printf("CPU's ebp: %x\n", ebp);
	printf("Function's retPC: %x, Function's oldFP: %x\n", ebp, ebp + 1);

	

	return v;
}

int save(void)
{
	int *ebp = getebp();

	printf("CPU's ebp: %x\n", ebp);

	savedFP = ebp;
	savedPC = ebp + 1;

	printf("retPC: %x, oldFP: %x\n", savedFP, savedPC);
	
	return 0;
}
