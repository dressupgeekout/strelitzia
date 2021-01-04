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

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <mruby.h>
#include <mruby/numeric.h>
#include <mruby/compile.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include "scriptlib.h"

#include "game.h"

bool
Game_GlobalSetup(void)
{
	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
	IMG_Init(IMG_INIT_PNG);
	Mix_Init(MIX_INIT_OGG);
	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, AUDIO_S16LSB, MIX_CHANNELS, 4096);
	TTF_Init();
	return true;
}


void
Game_GlobalShutdown(void)
{
	TTF_Quit();
	Mix_CloseAudio();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

/* ********** */

Game *
Game_Create(void)
{
	Game *game = malloc(sizeof(Game));
	if (!game) {
		goto fail;
	}

	snprintf(game->scriptdir, sizeof(game->scriptdir), "%s", "");
	game->renderer = NULL;
	game->R = NULL;
	game->current_scene = mrb_nil_value();
	game->music = NULL;
	game->ticks = 0;
	game->want_quit = false;
	game->scale = GAME_DEFAULT_SCALE;

	/*
	 * Set up the window where we'll be drawing.
	 */
	int rv = SDL_CreateWindowAndRenderer(GAME_DEFAULT_WIDTH*game->scale, GAME_DEFAULT_HEIGHT*game->scale, 0, &game->window, &game->renderer);

	if (rv < 0) {
		goto fail;
	}

	SDL_SetWindowTitle(game->window, "Strelitzia");

	/* 
	 * Fire up the mruby interpreter and load it with interesting functionality
	 * for scripts.
	 */
	if (!(game->R = mrb_open())) {
		goto fail;
	}

	return game;

fail:
	if (game) Game_Destroy(game);
	return NULL;
}


void
Game_Destroy(Game *game)
{
	if (game->window) SDL_DestroyWindow(game->window);
	if (game->renderer) SDL_DestroyRenderer(game->renderer);
	if (game->music) {
		Mix_HaltMusic();
		Mix_FreeMusic(game->music);
	}
	if (game->R) mrb_close(game->R);
	free(game);
}


void
Game_SetScriptDir(Game *game, const char *path)
{
	snprintf(game->scriptdir, sizeof(game->scriptdir), "%s", path);
	printf("Set scriptdir to %s\n", game->scriptdir);
}


void
Game_MainLoop(Game *game)
{
	bool done = false;
	SDL_Event event;

	while (!done) {
		/* Report any Ruby errors that might have happened. */
		if (game->R->exc) {
			mrb_print_backtrace(game->R);
			game->R->exc = NULL;
		}

		/* Compute the change in time. */
		if (!mrb_nil_p(game->current_scene)) {
			uint32_t now = SDL_GetTicks();
			uint32_t dt = now - game->ticks;
			mrb_funcall(game->R, game->current_scene, "update", 1, mrb_int_value(game->R, dt));
			game->ticks = now;
		}

		/* Handle input events. */
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				Game_RequestQuit(game, "SDL_QUIT");
				break;
			case SDL_KEYDOWN:
				Game_HandleKeydown(game, event.key.keysym.sym);
				break;
			case SDL_MOUSEBUTTONDOWN:
				Game_HandleMousedown(game, event.button);
				break;
			default:
				; /* OK */
			}
		}

		/* If a request to quit was made, handle it now. */
		if (game->want_quit) {
			done = true;
			if (!mrb_nil_p(game->current_scene)) {
				mrb_funcall(game->R, game->current_scene, "onunload", 0);
			}
			continue;
		}

		/* Draw stuff to screen! */
		if (!mrb_nil_p(game->current_scene)) {
			SDL_RenderClear(game->renderer);
			SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 0);
			mrb_funcall(game->R, game->current_scene, "draw", 0);
			SDL_RenderPresent(game->renderer);
		}

		/* LOVE2D does this to save a lot of CPU. */
		SDL_Delay(1);
	}
}

/* ********** */

void
Game_RequestQuit(Game *game, const char *reason)
{
	printf("Quit requested: %s\n", reason);
	game->want_quit = true;
}


/*
 * If a script is not loadable, then we quit.
 */
mrb_value
Game_LoadScript(Game *game, const char *path)
{
	static char real_path[PATH_MAX];
	snprintf(real_path, sizeof(real_path), "%s/%s", game->scriptdir, path);

	mrb_value obj = mrb_nil_value();

	printf("Loading %s\n", real_path);

	FILE *fp = fopen(real_path, "r");
	if (!fp) {
		return obj;
	}
	obj = mrb_load_file(game->R, fp);
	fclose(fp);

	if (game->R->exc) {
		mrb_print_backtrace(game->R);
		Game_RequestQuit(game, "couldn't load script");
		return mrb_nil_value();
	}

	return obj;
}


/*
 * XXX should verify we have an object kind_of? Scene
 *
 * Loading a new scene implies unloading the current one.
 */
void
Game_LoadScene(Game *game, const char *path)
{
	if (!mrb_nil_p(game->current_scene)) {
		mrb_funcall(game->R, game->current_scene, "onunload", 0);
	}

	mrb_value scene = Game_LoadScript(game, path);
	if (!mrb_nil_p(scene)) {
		game->current_scene = scene;
		mrb_funcall(game->R, game->current_scene, "onload", 0);
	}
}


/*
 * Loading new music implies unloading the current music.
 */
bool
Game_LoadMusic(Game *game, const char *path)
{
	if (game->music) {
		Mix_HaltMusic();
		Mix_FreeMusic(game->music);
	}

	game->music = Mix_LoadMUS(path);

	if (game->music) {
		return true;
	} else {
		fprintf(stderr, "%s: %s\n", __func__, Mix_GetError());
		return false;
	}
}


/*
 * XXX this always loops
 */
void
Game_PlayMusic(Game *game)
{
	if (!game->music) {
		return;
	}

	Mix_PlayMusic(game->music, -1);
}


void
Game_PauseMusic(Game *game)
{
	if (game->music) {
		Mix_PauseMusic();
	}
}


void
Game_HandleKeydown(Game *game, SDL_Keycode key)
{
	if (mrb_nil_p(game->current_scene))
		return;

	mrb_funcall(game->R, game->current_scene, "onkeydown", 1,
		mrb_str_new_cstr(game->R, SDL_GetKeyName(key))
	);
}


void
Game_HandleMousedown(Game *game, SDL_MouseButtonEvent event)
{
	if (mrb_nil_p(game->current_scene))
		return;

	/* I personally prefer to be explicit about this. */
	int real_button;
	switch (event.button) {
	case SDL_BUTTON_LEFT:
		real_button = 1;
		break;
	case SDL_BUTTON_MIDDLE:
		real_button = 2;
		break;
	case SDL_BUTTON_RIGHT:
		real_button = 3;
		break;
	default:
		real_button = 0;
		break;
	}

	mrb_funcall(game->R, game->current_scene, "onmousedown", 3,
		mrb_int_value(game->R, real_button),
		mrb_int_value(game->R, event.x),
		mrb_int_value(game->R, event.y)
	);
}
