OFFSTAGE = :OFFSTAGE
STAGE_LEFT = :STAGE_LEFT
CENTER_STAGE = :CENTER_STAGE
STAGE_RIGHT = :STAGE_RIGHT

class Character
  DEFAULT_HP = 100
  DEFAULT_NAME = "(anonymous)"

  attr_accessor :name
  attr_accessor :hp
  attr_accessor :portraits
  attr_accessor :current_portrait_name
  attr_accessor :inventory
  attr_accessor :position

  def initialize(**kwargs)
    @name = kwargs[:name] || DEFAULT_NAME
    hp = kwargs[:hp] || DEFAULT_HP
    @hp = {
      :max => hp,
      :current => hp,
    }
    @portraits = {}
    @inventory = {}
    @current_portrait_name = nil
    @position = OFFSTAGE
  end

  def add_portrait(name, path)
    @portraits[name] = load_image(path)
  end

  def use_portrait(name)
    @current_portrait_name = name
  end

  def current_portrait
    return @portraits[@current_portrait_name]
  end

  def offstage?
    return @position == OFFSTAGE
  end

  # Puts the character onstage.
  def enter(pos)
    @position = pos
  end

  # Puts the character offstge.
  def exit
    @position = OFFSTAGE
  end

  # XXX This assumes the character will be onstage.
  def draw
    w, h = screen_size
    y = 150

    case @position
    when STAGE_RIGHT
      x = 100
    when CENTER_STAGE
      x = w/2 - 200
    when STAGE_LEFT
      x = w - 200
    end

    blit_image(self.current_portrait, x, y)
  end

  # Adjusts the contents of the dialogue box represent what this character
  # is saying.
  #
  # We check for the name because the narrator has no name.
  def say(txt)
    str = @name ? "#{@name}: #{txt}" : txt
    $state[:dialogue_box].use_text(str)
  end
end
