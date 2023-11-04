
# pimp

Naive implementation of some adaptive filters in C. Also for ARM, so runs on [Bela](https://bela.io/).

The following algorithms are implemented (and tested!):

- Least-mean-squared filter
- Fast block-wise Least-mean-squared filter
- Recursive-Least-Squares filter

## Get started

Clone

	git clone https://github.com/fhchl/pimp.git
	cd pimp
	git submodule init && git submodule update

Build

	mkdir build
	cd build
	cmake ..
	make

Build for `float` instead of `double`

	cmake .. -D PIMP_USE_DOUBLE=0
	make

Build with [pocketfft](https://gitlab.mpcdf.mpg.de/mtr/pocketfft) for `BlockLMSFilter` on x86 (only supports `double`s)

	cmake .. -D PIMP_FFTLIB=pocketfft
	make

Build with [Ne10](https://projectne10.github.io/Ne10/) for `BlockLMSFilter` on ARM (only supports `float`s)

	cmake .. -D PIMP_FFTLIB=ne10
	make

Also build tests (requires python with numpy and scipy)

	cmake .. -D PIMP_BUILD_TESTS=1 -D PIMP_FFTLIB=pocketfft
	make

Run tests

	make test

## Cross compile for arm but test on x86

As described here: https://azeria-labs.com/arm-on-x86-qemu-user/

	qemu-arm -L /usr/arm-linux-gnueabihf ./test_fft


