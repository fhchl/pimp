
# pimp

Naive implementation of some adaptive filters in C, also for arm, so runs on [Bela](https://bela.io/).

## Get started

Clone

	git clone https://github.com/fhchl/pimp.git
	cd pimp
	git submodule init && git submodule update

Build

	mkdir build
	cd build
	cmake ..

Test

	make test

## Cross compile for arm but test on x86

As described here: https://azeria-labs.com/arm-on-x86-qemu-user/

	qemu-arm -L /usr/arm-linux-gnueabihf ./test_fft


