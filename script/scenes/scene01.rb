class TitleScreen < Scene
  def onload
    super
    @background = load_image("../data/title_bg.png")
    @flamingo = {
      :image => load_image("../data/flamingo-silhouette.png"),
      :x => 100,
      :y => 100,
    }
    @ticks = 0
    @seconds = 0
    @ticking = true
    @title_font = load_font("../data/eb-garamond/EBGaramond12-Regular.otf", 128)

    $state[:times_visited] ||= 0
    $state[:times_visited] += 1
    puts "You have visited the title screen #{$state[:times_visited]} times."

    $CHAR_KAISSA.enter(STAGE_RIGHT)

    load_music("../data/audio/music/music01.ogg")
    play_music
  end

  def draw
    fullw, fullh = $state[:screen_size]
    blit_image(@background, 0, 0, fullw, fullh)
    render_text(@title_font, "Firefly Lantern", 250, 100)
    blit_image(@flamingo[:image], @flamingo[:x], @flamingo[:y])
    $CHAR_KAISSA.draw if not $CHAR_KAISSA.offstage?

    set_color(255, 255, 0, 255)
    rectangle(200, 200, 300, 20)
  end

  def onunload
    super
    $CHAR_KAISSA.exit
    set_color(0, 0, 0, 0)
    pause_music
  end

  def onkeydown(key)
    super

    case key
    when "Up"
      @flamingo[:y] -= 100
    when "Down"
      @flamingo[:y] += 100
    when "Left"
      @flamingo[:x] -= 100
    when "Right"
      @flamingo[:x] += 100
    when "T"
      @ticking = !@ticking
      puts "new ticking status = #{@ticking.inspect}"
    when "Return"
      #load_scene "../script/scenes/scene02.rb"
      load_scene "../script/scenes/scene04.rb"
    when "S"
      if $CHAR_KAISSA.offstage?
        $CHAR_KAISSA.enter(STAGE_RIGHT)
      else
        $CHAR_KAISSA.exit
      end
    when "F"
      set_fullscreen(true)
    when "G"
      set_fullscreen(false)
    end
  end

  def update(dt)
    return if not @ticking
    @ticks += dt
    if @ticks >= 1000
      @ticks -= 1000
      @seconds += 1
      puts "seconds = #{@seconds}"
    end
  end
end

@scene = TitleScreen.new(name: "INTRO")

return @scene
