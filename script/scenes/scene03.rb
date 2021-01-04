@scene = StoryScene.new(
  name: "meeting_scene",
  actors: [
    $CHAR_KAISSA,
  ]
)

def kaissa(txt)
  @scene.beat {
    $CHAR_KAISSA.say(txt)
  }
end

@scene.beat {
  $CHAR_KAISSA.enter(CENTER_STAGE)
  $CHAR_KAISSA.say "Hello!"
}

kaissa "How are you?"
kaissa "I'm fine, how about yourself?"
kaissa "I'm splendid."
kaissa "This is a very long string that is definitely going to take
  multiple lines of text, but it should handle it just fine. I could
  probably handle it..."

@scene.beat {
  @scene.bg = load_image("../data/bg_001.png")
  $CHAR_KAISSA.say "Welcome."
}

@scene.beat {
  $CHAR_KAISSA.exit
  load_scene "../script/scenes/scene01.rb"
}

return @scene
