$ART_SWORDA = Artifact.new(name: "Sword A", weapon: true)
$ART_SWORDB = Artifact.new(name: "Sword B", weapon: true)

$ART_BEER = Artifact.new(
  name: "Beer",
  usage: proc { |art, user|
    puts "#{user.name} drinks the #{art.name}."
  },
)

$ART_SALVE = Artifact.new(
  name: "Salve",
  usage: proc { |art, user|
    puts "#{user.name} used the #{art.name}."
    user.hp += 50
  },
)
