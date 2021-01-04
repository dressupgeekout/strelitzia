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

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include <mruby.h>
#include <mruby/array.h>
#include <mruby/compile.h>
#include <mruby/data.h>
#include <mruby/hash.h>
#include <mruby/irep.h>
#include <mruby/numeric.h>
#include <mruby/variable.h>

#include "game.h"
#include "version.h"

#include "scriptlib.h"
#include <scene.h>

/* ********** */

/*
 * Here's how we make various C structures managed by Ruby.
 */
mrb_data_type RData_SDL_Texture = {
	.struct_name = "SDL_Texture",
	.dfree = SDL_Texture_dfree,
};

mrb_data_type RData_TTF_Font = {
	.struct_name = "TTF_Font",
	.dfree = TTF_Font_dfree,
};

void
SDL_Texture_dfree(mrb_state *R, void *ptr)
{
	SDL_DestroyTexture((SDL_Texture*)ptr);
}

void
TTF_Font_dfree(mrb_state *R, void *ptr)
{
	TTF_CloseFont((TTF_Font*)ptr);
}

/* ********** */

static Game *get_global_game(mrb_state *R);

static Game *
get_global_game(mrb_state *R)
{
	mrb_value game_obj = mrb_gv_get(R, mrb_intern_cstr(R, "$__game"));
	return mrb_cptr(game_obj);
}

/* ********** */

/*
 * blit_image(texture, x, y [, w, h]) # => nil
 */
mrb_value
Script_blit_image(mrb_state *R, mrb_value self)
{
	Game *game = get_global_game(R);

	mrb_value texture_obj;
	mrb_int x;
	mrb_int y;
	mrb_int w;
	mrb_bool w_given;
	mrb_int h;
	mrb_bool h_given;
	mrb_get_args(R, "oii|i?i?", &texture_obj, &x, &y, &w, &w_given, &h, &h_given);

	SDL_Texture *texture = DATA_GET_PTR(R, texture_obj, &RData_SDL_Texture, SDL_Texture);

	if (!(w_given && h_given)) {
		SDL_QueryTexture(texture, NULL, NULL, (int*)&w, (int*)&h);
	}

	SDL_RenderCopy(game->renderer, texture, NULL, &(SDL_Rect){x, y, w, h});

	return mrb_nil_value();
}


/*
 * img <SDL_Texture *> = load_image(path)
 */
mrb_value
Script_load_image(mrb_state *R, mrb_value self)
{
	Game *game = get_global_game(R);

	char *path = NULL;
	mrb_get_args(R, "z", &path);

	SDL_Surface *surf = IMG_Load((const char *)path);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(game->renderer, surf);
	SDL_FreeSurface(surf);

	return mrb_obj_value(Data_Wrap_Struct(game->R, game->R->object_class, &RData_SDL_Texture, texture));
}


/*
 * image_size(texture) # => [Fixnum, Fixnum]
 */
mrb_value
Script_image_size(mrb_state *R, mrb_value self)
{
	mrb_value texture_obj;

	mrb_get_args(R, "o", &texture_obj);

	SDL_Texture *texture = DATA_GET_PTR(R, texture_obj, &RData_SDL_Texture, SDL_Texture);

	int w;
	int h;
	SDL_QueryTexture(texture, NULL, NULL, &w, &h);

	mrb_value size_ary = mrb_ary_new_capa(R, 2);
	mrb_ary_set(R, size_ary, 0, mrb_int_value(R, w));
	mrb_ary_set(R, size_ary, 1, mrb_int_value(R, h));

	return size_ary;
}


/*
 * font <TTF_Font *> = load_font(path, size)
 */
mrb_value
Script_load_font(mrb_state *R, mrb_value self)
{
	char *path = NULL;
	mrb_int size;
	mrb_get_args(R, "zi", &path, &size);

	TTF_Font *font = TTF_OpenFont(path, size);
	return mrb_obj_value(Data_Wrap_Struct(R, R->object_class, &RData_TTF_Font, font));
}


/*
 * render_text(font, text, x, y) # => nil
 *
 * XXX we're mallocing and freeing a surface and a texture once per dt, which
 * honestly sucks
 */
mrb_value
Script_render_text(mrb_state *R, mrb_value self)
{
	Game *game = get_global_game(R);

	mrb_value font_obj;
	char *text = NULL;
	mrb_int x;
	mrb_int y;
	mrb_get_args(R, "ozii", &font_obj, &text, &x, &y);

	TTF_Font *font = DATA_GET_PTR(R, font_obj, &RData_TTF_Font, TTF_Font);

	SDL_Surface *surf = TTF_RenderText_Solid(font, (const char *)text, (SDL_Color){255, 255, 255, 255});
	//SDL_Surface *surf = TTF_RenderText_Blended(font, (const char *)text, (SDL_Color){255, 255, 255, 255});
	SDL_Texture *texture = SDL_CreateTextureFromSurface(game->renderer, surf);
	SDL_FreeSurface(surf);

	/*
	 * Compute how big the resulting texture is actually going to be, in pixels.
	 */
	int w;
	int h;
	TTF_SizeText(font, (const char *)text, &w, &h);

	SDL_RenderCopy(game->renderer, texture, NULL, &(SDL_Rect){x, y, w, h});

	SDL_DestroyTexture(texture);

	return mrb_nil_value();
}


/*
 * load_music(path) # => nil
 */
mrb_value
Script_load_music(mrb_state *R, mrb_value self)
{
	Game *game = get_global_game(R);
	char *path = NULL;

	mrb_get_args(R, "z", &path);

	if (!Game_LoadMusic(game, (const char *)path)) {
		/* XXX raise */
	}

	return mrb_nil_value();
}


/*
 * play_music() # => nil
 */
mrb_value
Script_play_music(mrb_state *R, mrb_value self)
{
	Game *game = get_global_game(R);
	Game_PlayMusic(game);
	return mrb_nil_value();
}


/*
 * pause_music() # => nil
 */
mrb_value
Script_pause_music(mrb_state *R, mrb_value self)
{
	Game *game = get_global_game(R);
	Game_PauseMusic(game);
	return mrb_nil_value();
}


/*
 * set_music_volume(n) # => nil
 */
mrb_value
Script_set_music_volume(mrb_state *R, mrb_value self)
{
	mrb_int n;
	mrb_get_args(R, "i", &n);
	Mix_VolumeMusic(n);
	return mrb_nil_value();
}


/*
 * set_color(r, g, b, a) # => nil
 */
mrb_value
Script_set_color(mrb_state *R, mrb_value self)
{
	Game *game = get_global_game(R);

	mrb_int r;
	mrb_int g;
	mrb_int b;
	mrb_int a;
	mrb_get_args(R, "iiii", &r, &g, &b, &a);

	SDL_SetRenderDrawColor(game->renderer, r, g, b, a);
	return mrb_nil_value();
}


/*
 * rectangle(x, y, w, h) # => nil
 */
mrb_value
Script_rectangle(mrb_state *R, mrb_value self)
{
	Game *game = get_global_game(R);

	mrb_int x;
	mrb_int y;
	mrb_int w;
	mrb_int h;
	mrb_get_args(R, "iiii", &x, &y, &w, &h);

	SDL_RenderFillRect(game->renderer, &(SDL_Rect){x, y, w, h});
	return mrb_nil_value();
}


/*
 * screen_size() # => [Fixnum, Fixnum]
 */
mrb_value
Script_screen_size(mrb_state *R, mrb_value self)
{
	Game *game = get_global_game(R);

	int w, h;
	SDL_GetWindowSize(game->window, &w, &h);

	mrb_value size_ary = mrb_ary_new_capa(R, 2);
	mrb_ary_set(R, size_ary, 0, mrb_int_value(R, w));
	mrb_ary_set(R, size_ary, 1, mrb_int_value(R, h));

	return size_ary;
}


/*
 * set_fullscreen(bool) # => nil
 */
mrb_value
Script_set_fullscreen(mrb_state *R, mrb_value self)
{
	Game *game = get_global_game(R);

	mrb_bool enable;
	mrb_get_args(R, "b", &enable);

	if (enable) {
		SDL_SetWindowFullscreen(game->window, SDL_WINDOW_FULLSCREEN);
	} else {
		SDL_SetWindowFullscreen(game->window, 0);
	}

	return mrb_nil_value();
}


/*
 * load_scene(path) # => nil
 */
mrb_value
Script_load_scene(mrb_state *R, mrb_value self)
{
	Game *game = get_global_game(R);

	char *path = NULL;
	mrb_get_args(R, "z", &path);

	Game_LoadScene(game, (const char *)path);

	return mrb_nil_value();
}


/*
 * request_quit(reason) # => nil
 */
mrb_value
Script_request_quit(mrb_state *R, mrb_value self)
{
	Game *game = get_global_game(R);

	char *reason = NULL;
	mrb_get_args(R, "z", &reason);

	Game_RequestQuit(game, (const char *)reason);

	return mrb_nil_value();
}

/* ********** */

void
Scriptlib_Init(Game *game)
{
	/* Global variables */
	mrb_gv_set(game->R, mrb_intern_cstr(game->R, "$__game"), mrb_cptr_value(game->R, game));
	mrb_gv_set(game->R, mrb_intern_cstr(game->R, "$state"), mrb_hash_new(game->R));
	mrb_gv_set(game->R, mrb_intern_cstr(game->R, "$STRELITZIA_VERSION"), mrb_str_new_cstr(game->R, STRELITZIA_VERSION));

	/* Kernel methods */
	mrb_define_method(game->R, game->R->kernel_module, "blit_image", Script_blit_image, MRB_ARGS_ARG(3, 2));
	mrb_define_method(game->R, game->R->kernel_module, "load_image", Script_load_image, MRB_ARGS_REQ(1));
	mrb_define_method(game->R, game->R->kernel_module, "image_size", Script_image_size, MRB_ARGS_REQ(1));

	mrb_define_method(game->R, game->R->kernel_module, "load_font", Script_load_font, MRB_ARGS_REQ(2));
	mrb_define_method(game->R, game->R->kernel_module, "render_text", Script_render_text, MRB_ARGS_REQ(4));

	mrb_define_method(game->R, game->R->kernel_module, "load_music", Script_load_music, MRB_ARGS_REQ(1));
	mrb_define_method(game->R, game->R->kernel_module, "play_music", Script_play_music, MRB_ARGS_NONE());
	mrb_define_method(game->R, game->R->kernel_module, "pause_music", Script_pause_music, MRB_ARGS_NONE());
	mrb_define_method(game->R, game->R->kernel_module, "set_music_volume", Script_set_music_volume, MRB_ARGS_REQ(1));

	mrb_define_method(game->R, game->R->kernel_module, "set_color", Script_set_color, MRB_ARGS_REQ(4));
	mrb_define_method(game->R, game->R->kernel_module, "rectangle", Script_rectangle, MRB_ARGS_REQ(4));

	mrb_define_method(game->R, game->R->kernel_module, "screen_size", Script_screen_size, MRB_ARGS_NONE());
	mrb_define_method(game->R, game->R->kernel_module, "set_fullscreen", Script_set_fullscreen, MRB_ARGS_REQ(1));
	mrb_define_method(game->R, game->R->kernel_module, "load_scene", Script_load_scene, MRB_ARGS_REQ(1));
	mrb_define_method(game->R, game->R->kernel_module, "request_quit", Script_request_quit, MRB_ARGS_REQ(1));

	/* Class definitions */
	mrb_load_irep(game->R, STRELITZIA_CLASS_SCENE);
}
