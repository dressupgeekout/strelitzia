# The BootSequence is always the very first scene launched by the game.
class BootSequence < Scene
  def onload
    super
    $state[:screen_size] = screen_size
    $state[:dialogue_box] = DialogueBox.new
    set_music_volume(128/4)
    $state[:player] = Character.new
    load_scene "../script/scenes/scene03.rb"
  end
end

return BootSequence.new(name: "__boot__")
