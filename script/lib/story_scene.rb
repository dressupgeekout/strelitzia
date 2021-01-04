# The StoryScene encapsulates when characters are talking to each other in
# "visual novel mode" (aka "storytime"). The script writer needs to define a
# list of beats. In most cases, last beat *should* load another scene.
class StoryScene < Scene
  attr_accessor :bg
  attr_accessor :beats

  def initialize(**kwargs)
    super
    @bg = kwargs[:bg] && load_image(kwargs[:bg])
    @beats = []
    @beat_counter = -1
    @actors = kwargs[:actors] || []
    @full_w, @full_h = screen_size
  end

  def onload
    super
    $state[:dialogue_box].show
    step
  end

  def onunload
    super
    $state[:dialogue_box].hide
  end

  def onkeydown(key)
    super

    case key
    when "9"
      puts "scene #{@name.inspect} => #{self.inspect}"
    when "Space", "Return"
      if @beat_counter < (@beats.length - 1)
        step
      end
    end
  end

  def draw
    blit_image(@bg, 0, 0, @full_w, @full_h) if @bg
    @actors.each do |actor|
      actor.draw if not actor.offstage?
    end
    $state[:dialogue_box].draw if $state[:dialogue_box].visible?
  end

  def step
    @beat_counter += 1
    @beats[@beat_counter].call
  end

  def beat(&block)
    @beats.push(block)
  end
end
