/*
 * Copyright (c) 2021 Charlotte Koch.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "game.h"
#include "scriptlib.h"
#include "version.h"

static void usage(void);
static void print_version(void);

int
main(int argc, char *argv[])
{
	int ch;
	char *scriptdir = NULL;

	while ((ch = getopt(argc, argv, "hs:v")) != -1) {
		switch (ch) {
		case 'h':
			usage();
			return EXIT_SUCCESS;
			break;
		case 's':
			scriptdir = optarg;
			break;
		case 'v':
			print_version();
			return EXIT_SUCCESS;
			break;
		case '?':
			usage();
			return EXIT_FAILURE;
			break;
		}
	}
	argc -= optind;
	argv += optind;

	if (!scriptdir) {
		fprintf(stderr, "a scriptdir is required, aborting\n");
		goto fail;
	}

	/* Ready to go! */
	print_version();

	Game *game = NULL;

	if (!Game_GlobalSetup()) {
		goto fail;
	}

	if (!(game = Game_Create())) {
		goto fail;
	}

	Game_SetScriptDir(game, scriptdir);

	Scriptlib_Init(game);
	Game_LoadScene(game, "scenes/boot.rb");

	Game_MainLoop(game);
	Game_Destroy(game);

	return EXIT_SUCCESS;

fail:
	if (game) Game_Destroy(game);
	Game_GlobalShutdown();
	return EXIT_FAILURE;
}


static void
usage(void)
{
	printf("%s: usage: %s [-v] [-s SCRIPTDIR]\n", getprogname(), getprogname());
}


static void
print_version(void)
{
	printf("*** STRELITZIA ENGINE %s ***\n", STRELITZIA_VERSION);
}
