# Describes a battle between the player and 1 or more enemies.
class BattleScene < Scene
  attr_accessor :belligerents

  def initialize(**kwargs)
    super
    @belligerents = kwargs[:belligerents]
    @player_turn = false
    @ticks = 0
  end

  def onkeydown(key)
    super

    if key == "Return"
      @player_turn = !@player_turn
      if @player_turn
        puts "Now it's your turn."
      else
        puts "Now it's #{@belligerents[0].name}'s turn."
      end
    end

    if key == "Backspace"
      load_scene "../script/scenes/scene01.rb"
    end
  end

  def update(dt)
    @ticks += dt

    if @ticks > 250
      @ticks -= 250
      if @belligerents[0].current_portrait_name == "red" 
        @belligerents[0].use_portrait("purple")
      else
        @belligerents[0].use_portrait("red")
      end
    end
  end

  def draw
    w, h = screen_size
    blit_image(@belligerents[0].current_portrait, (w/2)-(384/2), (h/2)-(386/2))
  end
end
