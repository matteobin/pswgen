#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#ifdef WIN32
#include <Windows.h>
#endif

/* macros */
#define LONG_OPT_NAME_LOWER "case-insensitive"
#define LONG_OPT_NAME_COMPLEX "complex"
#define LONG_OPT_NAME_HELP "help"
#define LONG_OPT_NAME_INCLUDE "include"
#define LONG_OPT_NAME_LENGTH "length"
#define LONG_OPT_NAME_NUMONLY "numbers-only"
#define LONG_OPT_NAME_VERSION "version"
#ifdef WIN32
#define LONG_OPT_PREFIX "/"
#else
#define LONG_OPT_PREFIX "--"
#endif
#define LONG_OPT_LOWER LONG_OPT_PREFIX LONG_OPT_NAME_LOWER
#define LONG_OPT_COMPLEX LONG_OPT_PREFIX LONG_OPT_NAME_COMPLEX
#define LONG_OPT_HELP LONG_OPT_PREFIX LONG_OPT_NAME_HELP
#define LONG_OPT_INCLUDE LONG_OPT_PREFIX LONG_OPT_NAME_INCLUDE
#define LONG_OPT_LENGTH LONG_OPT_PREFIX LONG_OPT_NAME_LENGTH
#define LONG_OPT_NUMONLY LONG_OPT_PREFIX LONG_OPT_NAME_NUMONLY
#define LONG_OPT_VERSION LONG_OPT_PREFIX LONG_OPT_NAME_VERSION
#define MAX_INCLUDE_CHARS 8
#define MAX_LENGTH 128
#ifdef WIN32
#define NAME "PSWGen"
#else
#define NAME "pswgen"
#endif
#define PSW_SIZE MAX_LENGTH + 1
#define RAND_MAX_LENGTH 64
#define RAND_MIN_LENGTH 24
#define SHORT_OPT_NAME_LOWER "c"
#define SHORT_OPT_NAME_COMPLEX "C"
#ifdef WIN32
#define SHORT_OPT_NAME_HELP "?"
#else
#define SHORT_OPT_NAME_HELP "h"
#endif
#define SHORT_OPT_NAME_INCLUDE "i"
#define SHORT_OPT_NAME_LENGTH "l"
#define SHORT_OPT_NAME_NUMONLY "n"
#define SHORT_OPT_NAME_VERSION "v"
#ifdef WIN32
#define SHORT_OPT_PREFIX "/"
#else
#define SHORT_OPT_PREFIX "-"
#endif
#define SHORT_OPT_LOWER SHORT_OPT_PREFIX SHORT_OPT_NAME_LOWER
#define SHORT_OPT_COMPLEX SHORT_OPT_PREFIX SHORT_OPT_NAME_COMPLEX
#define SHORT_OPT_HELP SHORT_OPT_PREFIX SHORT_OPT_NAME_HELP
#define SHORT_OPT_INCLUDE SHORT_OPT_PREFIX SHORT_OPT_NAME_INCLUDE
#define SHORT_OPT_LENGTH SHORT_OPT_PREFIX SHORT_OPT_NAME_LENGTH
#define SHORT_OPT_NUMONLY SHORT_OPT_PREFIX SHORT_OPT_NAME_NUMONLY
#define SHORT_OPT_VERSION SHORT_OPT_PREFIX SHORT_OPT_NAME_VERSION
#define SPECIAL_CHARS " !\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"

/* function declarations */
static void printerr(char msg[]);
static void printhelp(void);
static void printversion(void);
static int randnum(int min, int max);

/* variables */
static char chars[106] = "0123456789"; // 10 + 26 + 26 + 35 + 8 + 1
static int complex = 0;
static int length = 0;
static int lower = 0;
static int numonly = 0;

/* function implementations */
void
printerr(char msg[])
{
	fprintf(stderr, "FATAL: %s.\n", msg);
	exit(EXIT_FAILURE);
}

void
printhelp()
{
	printf("\
Usage: %s [OPTIONS]\n\
Generate password and print it to stdout", NAME);
	#ifdef WIN32
	printf(",\nthen copy it to clipboard for easy pasting.\n");
	#else
	printf(".\n");
	#endif
	printf("\n\
Options:\n\
%s, %s\tlowercase password\n\
%s, %s\t\tadd \"special\" characters*\n\
%s, %s\n\
%s, %s CHAR(s)\t%3i max\n\
%s, %s NUMBER\t%3i max\n\
%s, %s\n\
%s, %s\n\n\
*Special characters:\t%s\n\n\
By default, length is a random number within %i and %i.\n",\
SHORT_OPT_LOWER, LONG_OPT_LOWER,\
SHORT_OPT_COMPLEX, LONG_OPT_COMPLEX,\
SHORT_OPT_HELP, LONG_OPT_HELP,\
SHORT_OPT_INCLUDE, LONG_OPT_INCLUDE, MAX_INCLUDE_CHARS,\
SHORT_OPT_LENGTH, LONG_OPT_LENGTH, MAX_LENGTH,\
SHORT_OPT_NUMONLY, LONG_OPT_NUMONLY,\
SHORT_OPT_VERSION, LONG_OPT_VERSION,\
SPECIAL_CHARS,\
RAND_MIN_LENGTH, RAND_MAX_LENGTH);
	exit(EXIT_SUCCESS);
}

void
printversion()
{
	printf("%s 3.2\n\
Copyright (c) 2022 Matteo Bini\n\
License: Apache-2.0 <https://www.apache.org/licenses/LICENSE-2.0>.\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n\n\
Written by Matteo Bini.\n", NAME);
	exit(EXIT_SUCCESS);
}

int
randnum(int min, int max)
{
	int range = max - min + 1;
	int remainder = RAND_MAX % range;
	int n;
	do {
		n = rand();
	} while (n >= RAND_MAX - remainder);
	return min + n % range;
}

int
main(int argc, char* argv[])
{
	int i;
	char msg[72] = "";
	char inchars[MAX_INCLUDE_CHARS + 1] = "";
	int charsli;
	char psw[PSW_SIZE] = "";
	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], SHORT_OPT_HELP) || !strcmp(argv[i], LONG_OPT_HELP))
			printhelp();
		else if (!strcmp(argv[i], SHORT_OPT_VERSION) || !strcmp(argv[i], LONG_OPT_VERSION))
			printversion();
		else if (!strcmp(argv[i], SHORT_OPT_LOWER) || !strcmp(argv[i], LONG_OPT_LOWER))
			lower = 1;
		else if (!strcmp(argv[i], SHORT_OPT_COMPLEX) || !strcmp(argv[i], LONG_OPT_COMPLEX))
			complex = 1;
		else if (!strcmp(argv[i], SHORT_OPT_INCLUDE) || !strcmp(argv[i], LONG_OPT_INCLUDE)) {
			if (++i >= argc)
				printerr("include option requires at least 1 character");
			if (strlen(argv[i]) > MAX_INCLUDE_CHARS) {
				sprintf(msg, "include option accepts %i characters max", MAX_INCLUDE_CHARS);
				printerr(msg);
			}
			sscanf(argv[i], "%s", &inchars);
		}
		else if (!strcmp(argv[i], SHORT_OPT_LENGTH) || !strcmp(argv[i], LONG_OPT_LENGTH)) {
				sprintf(msg, "length option requires a number between 1 and %i", MAX_LENGTH);
				if (++i >= argc)
					printerr(msg);
				if (strlen(argv[i]) > 3)
					printerr(msg);
				sscanf(argv[i], "%i", &length);
				if (length < 1 || length > MAX_LENGTH)
					printerr(msg);
		}
		else if (!strcmp(argv[i], SHORT_OPT_NUMONLY) || !strcmp(argv[i], LONG_OPT_NUMONLY))
			numonly = 1;
		else
			printhelp();
	}
	strcat(chars, inchars);
	if (!numonly) {
		strcat(chars, "abcdefghijklmnopqrstuvwxyz");
		if (!lower)
			strcat(chars, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
		if (complex)
			strcat(chars, SPECIAL_CHARS);
	}
	srand(time(NULL));
	if (!length)
		length = randnum(RAND_MIN_LENGTH, RAND_MAX_LENGTH);
	charsli = strlen(chars) - 1;
	for (i = 0; i < length; i++)
		psw[i] = chars[randnum(0, charsli)];
	printf("%s\n", psw);
	#ifdef WIN32
	HGLOBAL hmem = GlobalAlloc(GMEM_MOVEABLE, PSW_SIZE);
	memcpy(GlobalLock(hmem), psw, PSW_SIZE);
	GlobalUnlock(hmem);
	OpenClipboard(0);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hmem);
	CloseClipboard();
	#endif
	return EXIT_SUCCESS;
}
