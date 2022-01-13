
# pimp

Naive implementation of some adaptive filters in C, also for arm, so runs on [Bela](https://bela.io/).

## Get started

Clone

	git clone https://github.com/fhchl/pimp.git
	cd pimp
	git submodule init && git submodule update

Build without fft support

	mkdir build
	cd build
	cmake ..
	make

Build with [pocketfft](https://gitlab.mpcdf.mpg.de/mtr/pocketfft) for `BlockLMSFilter` on x86

	cmake .. -D PIMP_FFTLIB=pocketfft
	make

Build with [Ne10](https://projectne10.github.io/Ne10/) for `BlockLMSFilter` on ARM

	cmake .. -D PIMP_FFTLIB=pocketfft
	make

Build for `float` instead of `double`

	cmake .. -D PIMP_USE_DOUBLE=0
	make

Run tests

	make test

## Cross compile for arm but test on x86

As described here: https://azeria-labs.com/arm-on-x86-qemu-user/

	qemu-arm -L /usr/arm-linux-gnueabihf ./test_fft


