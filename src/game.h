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

#ifndef STRELITZIA_GAME_H
#define STRELITZIA_GAME_H

#include <limits.h>
#include <stdint.h>
#include <stdbool.h>

#include <mruby.h>
#include <SDL.h>
#include <SDL_mixer.h>

#define GAME_DEFAULT_WIDTH 640
#define GAME_DEFAULT_HEIGHT 360
#define GAME_DEFAULT_SCALE 2

#define GAME_DATADIR "../data" /* XXX */

struct Game {
	char scriptdir[PATH_MAX];
	SDL_Window *window;
	SDL_Renderer *renderer;

	mrb_state *R;
	mrb_value current_scene;
	Mix_Music *music;

	uint32_t ticks;
	bool want_quit;
	size_t scale;
};
typedef struct Game Game;

bool Game_GlobalSetup(void);
void Game_GlobalShutdown(void);

Game *Game_Create(void);
void Game_Destroy(Game *game);
void Game_SetScriptDir(Game *game, const char *path);
void Game_MainLoop(Game *game);

void Game_RequestQuit(Game *game, const char *reason);
mrb_value Game_LoadScript(Game *game, const char *path);
void Game_LoadScene(Game *game, const char *path);

bool Game_LoadMusic(Game *game, const char *path);
void Game_PlayMusic(Game *game);
void Game_PauseMusic(Game *game);

void Game_HandleKeydown(Game *game, SDL_Keycode key);
void Game_HandleMousedown(Game *game, SDL_MouseButtonEvent event);

#endif /* STRELITZIA_GAME_H */
