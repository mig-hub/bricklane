SHELL := /bin/zsh
CFLAGS=-Wall -Wno-int-to-void-pointer-cast -Wno-unsequenced

all: bricklane
test:
	bash <(curl -fsSL http://git.io/roundup.sh) spec/*
clean:
	rm -f bricklane

