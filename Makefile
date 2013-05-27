SHELL := /bin/zsh

test:
	bash <(curl -fsSL http://git.io/roundup.sh) spec/*

