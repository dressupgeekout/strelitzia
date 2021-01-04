$CHAR_NARRATOR = Character.new
$CHAR_NARRATOR.name = nil # on purpose

$CHAR_KAISSA = Character.new(name: "Kaissa")
$CHAR_KAISSA.add_portrait("red", "../data/Magma-Dragon.png")
$CHAR_KAISSA.add_portrait("purple", "../data/Magma-Dragon2.png")
$CHAR_KAISSA.use_portrait("red")

$CHAR_SESH = Character.new(name: "Sesh", hp: 250)

$CHAR_EMPRESS = Character.new(name: "Empress", hp: 5000)


$CHAR_BUDDY = Character.new(name: "Buddy", hp: 100)
