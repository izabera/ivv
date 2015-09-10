/*
 * Entropy calculation and analysis of putative random sequences.
 *
 * Designed and implemented by John "Random" Walker in May 1985.
 *
 * Multiple analyses of random sequences added in December 1985.
 *
 * Bit stream analysis added in September 1997.
 *
 * getopt() command line processing
 * and HTML documentation added in October 1998.
 *
 * Replaced table look-up for chi square to probability
 * conversion with algorithmic computation in January 2008.
 *
 * For additional information and the latest version,
 * see http://www.fourmilab.ch/random/
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <unistd.h>

#include "randtest.h"

#define UPDATE "January 28th, 2008"

#define FALSE 0
#define TRUE  1

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Print information on how to call */
static void
help(void)
{
	printf("ent --  Calculate entropy of file.  Call");
	printf("\n        with ent [options] [input-file]");
	printf("\n");
	printf("\n        Options:   -b   Treat input as a stream of bits");
	printf("\n                   -u   Print this message\n");
	printf("\nBy John Walker");
	printf("\n   http://www.fourmilab.ch/");
	printf("\n   %s\n", UPDATE);
}

int
main(int argc, char *argv[])
{
	struct rt_stats r;

	int binary = FALSE; /* Treat input as a bitstream */

	{
		int opt;

		while (opt = getopt(argc, argv, "bu?"), opt != -1) {
			switch (opt) {
			case 'b': binary = TRUE; break;

			case '?':
			case 'u':
				help();
				return 0;
			}
		}

		argc -= optind;
		argv += optind;

		if (argc != 0) {
			help();
			return 2;
		}
	}

	rt_init(binary);

	{
		FILE *f = stdin;
		int c;

		while (c = fgetc(f), c != EOF) {
			unsigned char u = (unsigned char) c;
			rt_add(&u, 1);
		}

		fclose(f);
	}

	rt_end(&r);

	{
		const char *samp = binary ? "bit" : "byte";

		printf("Entropy = %f bits per %s.\n", r.ent, samp);
		printf("Chi square distribution is %1.2f,\n", r.chisq);
		printf("and randomly would exceed this value ");
		if (r.chip < 0.0001) {
			printf("less than 0.01 percent of the times.\n\n");
		} else if (r.chip > 0.9999) {
			printf("more than than 99.99 percent of the times.\n\n");
		} else {
			printf("%1.2f percent of the times.\n\n", r.chip * 100);
		}

		printf(
			"Arithmetic mean value of data %ss is %1.4f (%.1f = random).\n",
			samp, r.mean, binary ? 0.5 : 127.5);
			printf("Monte Carlo value for Pi is %1.9f (error %1.2f percent).\n",
			r.montepi, 100.0 * (fabs(M_PI - r.montepi) / M_PI));

		printf("Serial correlation coefficient is ");
		if (r.scc >= -99999) {
			printf("%1.6f (totally uncorrelated = 0.0).\n", r.scc);
		} else {
			printf("undefined (all values equal!).\n");
		}
	}

	return 0;
}

