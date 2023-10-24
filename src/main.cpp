#include "pch.h"
#include <string>
#include <ctime>
#include <stdlib.h>

#include "launch/run.h"

bool REPLMode = true;

int main(int argc, char **argv) {
	// `/dev/random` only exists on Unix.
#ifndef WIN32
	std::FILE *fd = std::fopen("/dev/random", "r");
	unsigned int seed = 0;
	// Read the first 4 bytes of /dev/random into an unsigned integer so that we can seed the random number generator.
	fread(&seed, 4, 1, fd);
	fclose(fd);
	srand(seed);
#else
	// FIXME: Better seeding on the niche gaming operating system Microsoft Windows.
	srand((unsigned int) time(nullptr));
#endif
	bool status;
	if (argc == 1) {
		status = startREPL();
	}
    if (argc == 2) {
        REPLMode = false;
		std::filesystem::path filepath(argv[1]);
        status = runFile(filepath);
    } else if (argc > 2) {
        std::cerr << "Too many arguments!\nUsage:\n" << argv[0] << " <filename>" << std::endl;
        return EXIT_FAILURE;
    }


    return status ? EXIT_SUCCESS : EXIT_FAILURE;
}
