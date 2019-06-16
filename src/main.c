
#include "dec2bin.h"

enum {
	NO_LICENSE_INFO,
	LICENSE_INFO
};

void print_license_info(void) {
	printf("This program is free software: you can redistribute it and/or modify\n"
    "it under the terms of the GNU General Public License as published by\n"
    "the Free Software Foundation, either version 3 of the License, or\n"
    "(at your option) any later version.\n\n"
    "This program is distributed in the hope that it will be useful,\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    "GNU General Public License for more details.\n\n"
    "You should have received a copy of the GNU General Public License\n"
    "along with this program.  If not, see <https://www.gnu.org/licenses/>.\n\n");
}

void print_license_info_short(void) {
	printf("This program comes with ABSOLUTELY NO WARRANTY; for details type `--version'.\n"
    "This is free software, and you are welcome to redistribute it\n"
    "under certain conditions; type `show c' for details.\n\n");
}

void print_version_info(int license) {
	printf("dec2bin Version 1.0.1\n");
	printf("Copyright (C) 2019 Jose Fernando Lopez Fernandez\n\n");
	
	if (license) {
		print_license_info();
	} else {
		print_license_info_short();
	}
}

void print_help(void) {
	print_version_info(NO_LICENSE_INFO);
	
	printf("Program Options:\n\n");
	printf("  -h, --help              Print this help menu and exit\n");
	printf("      --version           Print program version info and exit\n");
	printf("      --pretty-print      Print binary representation separated by groups\n");
	printf("      --group-size   [N]  Print binary representation in groups of N (default 4)\n");
	printf("\n");
}

int main(int argc, char *argv[])
{
    // Verify the user supplied arguments.
	if (argc == 1) {
        fprintf(stderr, "[Error] No input(s).\n");
        return EXIT_SUCCESS;
    }

	// Available options
	int option_pretty_print = FALSE;
	int option_pretty_print_group_size_specified = FALSE;
	int option_pretty_print_group_separator = ' '; // TODO: Implement option

	// Options configuration
	size_t option_pretty_print_group_size = 4;

	// Parse options
	for (size_t i = 1; i < (size_t) argc; ++i) {
		// If the first character of the current input string is not a dash,
		// don't bother checking it; it's not an option.
		if (argv[i][0] != '-') {
			// Move on with our lives, specifically to the next input string.
			continue;
		}

		// Handle command-line option "--help" or "-h"
		if (strcmp(argv[i],"-h") == 0 || strcmp(argv[i],"--help") == 0) {
			print_help();
			return EXIT_SUCCESS;
		}

		// Handle command-line option "--version"
		if (strcmp(argv[i],"--version") == 0) {
			print_version_info(LICENSE_INFO);
			return EXIT_SUCCESS;
		}

		// Handle command-line option "--pretty-print". This option causes the
		// binary string output to be printed in groups of 4.
		//
		// TODO: Allow user to specify output group size
		if (strcmp(argv[i],"--pretty-print") == 0) {
			option_pretty_print = TRUE;
		} else if (strcmp(argv[i],"--group-size") == 0) {
			option_pretty_print_group_size_specified = TRUE;

			// Verify the user supplied an actual argument for this option, and
			// didn't simply pass in a blank value for the group. We have to
			// check that there are at least two more options because otherwise
			// the group size specifier will be parsed as a number to interpret.
			//
			// If no value was passed in, short-circuit and exit with an error
			// return code.
			if (i + 2 >= (size_t) argc) {
				fprintf(stderr, "[Error] The '--group-size' option requires an actual value to be specified (i.e... '--group-size 8').\n");
				return EXIT_FAILURE;
			}

			// If there is another in the input string, parse it as the value.
			errno = 0;

			long val = strtol(argv[i+1], NULL, 10);

			// Verify the parsing succeeded successfully.
			if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0)) {
				perror("strtol");
				return EXIT_FAILURE;
			}

			// If we get here, we successfully parsed the number. For purposes
			// of error checking, 'val' had to be preserved as a 'long', but
			// having confirmed everything succeeded, we can now cast it.
			option_pretty_print_group_size = (size_t) val;
		} else {
			// If we hit this point, it means the supplied option was not
			// recognized.
			fprintf(stderr, "[Error] Option not recognized: %s\n", argv[i]);
			return EXIT_FAILURE;
		}
	}

	// Declare and initialize GMP variables only once.
	mpz_t n;
	mpz_init(n);

    while (*++argv) {
		// The group size specifier is a numeric value, so we have to check for
		// the '--group-size' option specifically, before checking for a string
		// beginning with a dash, otherwise the group size specifier will be
		// interpreted as input requiring conversion to binary.
		//
		// The group_size_specified flag is first to short circuit the check if
		// that option was not actually set by the user. That way the 'strcmp'
		// check is only performed if it's actually necessary.
		if (option_pretty_print_group_size_specified && (strcmp(*argv,"--group-size") == 0)) {
			// If the current string is the '--group-size' option, increment
			// the 'argv' pointer. It will now be pointing at the numeric input
			// representing the actual group size string.
			++argv;

			// Now simply terminate the current loop iteration, at which point
			// the 'while' loop will check its stop condition. This check
			// involves pre-incrementing the 'argv' pointer, so this results
			// in a total shift of two, which is the desired action.
			continue;
		}

		// Having parsed the options already. Assume any input string beginning
		// with a dash ('-') is an option and skip it.
		if ((*argv)[0] == '-') {
			continue;
		}

		// As a performance optimization the GMP variable is declared, 
		// initialized, allocated, and deallocated only once. The value is
		// simply set at the beginning of each iteration.
        mpz_set_str(n, *argv, 10);

		// Check if user requested pretty printing. If they did, handle it.
		if (option_pretty_print == TRUE) {
			// To pretty print, get the string representation explicitly by
			// allocating it to 'num_str'. The 'NULL' argument in 'mpz_get_str'
			// means we did not pre-allocate a buffer for the string, so the
			// function is free to allocate it itself and return a pointer to
			// the string's location.
			const char* num_str = mpz_get_str(NULL, 2, n);

			// The width of the output should be a multiple of four, so if
			// the length of the string modulo four is not zero, the length of
			// the output should be corrected by incrementing the output
			// length.
			size_t output_length = strlen(num_str);

			while (output_length % option_pretty_print_group_size != 0) {
				++output_length;
			}

			// This counter ensures the proper number of digits are printed
			// per group.
			size_t digits_in_current_group = 0;

			// The difference between 'output_length' and 'strlen(num_str)'
			// will be the number of zeroes that must be prepended to the 
			// output.
			for (size_t i = 0; i < output_length - strlen(num_str); ++i) {
				if (digits_in_current_group == option_pretty_print_group_size) {
					printf("%c", option_pretty_print_group_separator);
					digits_in_current_group = 0;
				}

				// Increment group digits counter
				++digits_in_current_group;

				// Prepend zero to output
				printf("0");
			}

			// Having prepended the necessary padding, output the actual string
			// representation of the number.
			for (size_t i = 0; i < strlen(num_str); ++i) {
				if (digits_in_current_group == option_pretty_print_group_size) {
					printf("%c", option_pretty_print_group_separator);
					digits_in_current_group = 0;
				}

				// Increment group digits counter
				++digits_in_current_group;

				// Print out the current digit
				printf("%c", num_str[i]);
			}
		} else {
			// If the user did not request pretty printing the output, simply
			// write the string representation to stdout.
			mpz_out_str(stdout, 2, n);
		}

		// Print final newline
		printf("\n");
    }

	// Deallocate GMP variable memory only once.
	mpz_clear(n);

    return EXIT_SUCCESS;
}

