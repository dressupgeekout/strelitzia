@scene = StoryScene.new(
  name: "scene",
  actors: [
    $CHAR_KAISSA,
    $CHAR_SESH,
  ]
)

def kaissa(txt)
  @scene.beat {
    $CHAR_KAISSA.say(txt)
  }
end

def sesh(txt)
  @scene.beat {
    $CHAR_SESH.say(txt)
  }
end

def narrate(txt)
  @scene.beat {
    $CHAR_NARRATOR.say(txt)
  }
end

narrate "It's been a long night."
narrate "You try to figure out what's going on..."

kaissa "Hey."

sesh "Yo, what's up?"

@scene.beat {
  load_scene "../script/scenes/scene01.rb"
}

return @scene
