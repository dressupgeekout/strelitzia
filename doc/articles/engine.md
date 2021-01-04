# Engine Details

The Strelitzia engine is written in plain C against the following
libraries/frameworks:

- [SDL2][libsdl]
- [SDL2_image][sdl2_image]
- [SDL2_mixer][sdl2_mixer]
- [SDL2_ttf][sdl2_ttf]
- [mruby][mruby]
- [oniguruma][oniguruma]

In theory, any configuration of mruby is valid, but Strelitza is currently configured to use:

- mruby 2.1.2's "default" gembox, and
- the Regexp library implemented with [oniguruma][oniguruma]

[libsdl]: https://libsdl.org/
[mruby]: http://mruby.org/
[oniguruma]: https://github.com/kkos/oniguruma
[sdl2_mixer]: https://www.libsdl.org/projects/SDL_mixer/
[sdl2_ttf]: https://www.libsdl.org/projects/SDL_ttf/
[sdl2_image]: https://www.libsdl.org/projects/SDL_image/
