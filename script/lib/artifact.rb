class Artifact
  attr_reader :name
  attr_reader :descr
  attr_reader :weapon
  attr_reader :usage

  DEFAULT_USAGE = proc { |art, user| true }

  def initialize(**kwargs)
    @name = kwargs[:name]
    @descr = kwargs[:descr]
    @weapon = kwargs[:weapon]
    @usage = kwargs[:usage] || DEFAULT_USAGE
    @used = false
  end

  def weapon?
    return !!@weapon
  end

  def use(user)
    if !self.weapon? && @used
      # XXX complain
    else
      @usage.call(self, user)
      @used = true
    end
  end
end
