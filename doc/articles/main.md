**Strelitzia** is a simple game engine which allows users to write
multimedia applications in Ruby.


## Scene-oriented architecture

Strelitzia applications are composed of one or more _scenes_. A scene is a
very loose, abstract notion where

Scenes do not need to be presented in specific order. 


## Ruby API

The following methods are expected to be provided by your scripts:

- `draw`
- `onkeydown`
- `onload`
- `onmousedown`
- `onunload`
- `update`

The following methods are available for use in your scripts:

- `blit_image`
- `image_size`
- `load_font`
- `load_image`
- `load_music`
- `load_scene`
- `pause_music`
- `play_music`
- `rectangle`
- `render_text`
- `request_quit`
- `screen_size`
- `set_color`
- `set_fullscreen`
- `set_music_volume`
