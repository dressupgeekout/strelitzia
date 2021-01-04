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

/*
 * These are all the methods which are available to Ruby scripts but are
 * implemented in C.
 */

#ifndef STRELITZIA_SCRIPTLIB_H
#define STRELITZIA_SCRIPTLIB_H

#include <mruby.h>
#include <mruby/data.h>

#include "game.h"

extern mrb_data_type RData_SDL_Texture;
extern mrb_data_type RData_TTF_Font;
void SDL_Texture_dfree(mrb_state *R, void *ptr);
void TTF_Font_dfree(mrb_state *R, void *ptr);

mrb_value Script_blit_image(mrb_state *R, mrb_value self);
mrb_value Script_load_image(mrb_state *R, mrb_value self);
mrb_value Script_image_size(mrb_state *R, mrb_value self);

mrb_value Script_load_font(mrb_state *R, mrb_value self);
mrb_value Script_render_text(mrb_state *R, mrb_value self);

mrb_value Script_load_music(mrb_state *R, mrb_value self);
mrb_value Script_play_music(mrb_state *R, mrb_value self);
mrb_value Script_pause_music(mrb_state *R, mrb_value self);
mrb_value Script_set_music_volume(mrb_state *R, mrb_value self);

mrb_value Script_set_color(mrb_state *R, mrb_value self);
mrb_value Script_rectangle(mrb_state *R, mrb_value self);

mrb_value Script_screen_size(mrb_state *R, mrb_value self);
mrb_value Script_set_fullscreen(mrb_state *R, mrb_value self);
mrb_value Script_load_scene(mrb_state *R, mrb_value self);
mrb_value Script_request_quit(mrb_state *R, mrb_value self);

void Scriptlib_Init(Game *game);

#endif /* STRELITZIA_SCRIPTLIB_H */
