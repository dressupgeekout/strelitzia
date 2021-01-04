# There is only one DialogueBox. It's up to you to change its contents.
class DialogueBox
  MAX_LINE_LENGTH = 72
  MAX_LINES = 3

  def initialize
    @text = [""]
    @font_size = 42
    @font = load_font("../data/eb-garamond/EBGaramond12-Regular.otf", @font_size)
    @visible = false
    compute_text_positions
  end

  def compute_text_positions
    @text_positions = []
    @box_position = []

    w, h = $state[:screen_size]
    boxw = w*0.85
    boxh = (@font_size*3.5)
    boxx = (w - boxw)/2
    y = h - boxh
    margin = 16

    (0...MAX_LINES).map { |i| @text_positions[i] = [boxx+margin, y + (i*@font_size)] }
    @box_position = [boxx, y-margin, boxw, boxh]
  end
  private :compute_text_positions

  def visible?
    return !!@visible
  end

  def show
    @visible = true
  end

  def hide
    @visible = false
  end

  # This is a hard-word-wrapping routine.
  def split_text(str)
    this_line = 0
    lines = [""]

    str.split(/\s+/).each do |word|
      resulting_length = lines[this_line].length + word.length + 1 # 1 for space 

      if resulting_length > MAX_LINE_LENGTH
        this_line += 1
      end

      lines[this_line] = "#{lines[this_line]} #{word}"
    end

    return lines
  end
  private :split_text

  def use_text(text)
    @text = split_text(text)
  end

  def draw
    set_color(50, 50, 50, 255)
    rectangle(*@box_position)
    set_color(0, 0, 0, 0)

    @text.each_with_index do |line, i|
      render_text(@font, line, *@text_positions[i])
    end
  end
end
