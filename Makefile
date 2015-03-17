OUT=bin/gh-get.exe
CC=gcc

all: gh-get
	
gh-get: 
	$(CC) gh-get.c -o $(OUT)