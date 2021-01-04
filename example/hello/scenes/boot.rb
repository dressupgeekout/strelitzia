class ExampleScene < Scene
  def initialize
    @name = "EXAMPLE"
  end

  def onload
    puts "Hello from Strelitzia #{$STRELITZIA_VERSION}"
  end
end

return ExampleScene.new
