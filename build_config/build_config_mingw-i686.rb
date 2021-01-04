MRuby::CrossBuild.new("mingw-i686") do |conf|
  toolchain :gcc
  enable_debug

  conf.cc do |cc|
    cc.command = "i686-w64-mingw32-gcc"
    cc.include_paths << "#{MRUBY_ROOT}/../oniguruma/src"
  end

  conf.linker do |ld|
    ld.command = "i686-w64-mingw32-gcc"
    ld.link_options = %Q(%{flags} -o "%{outfile}" %{objs} %{libs})
    ld.library_paths << "#{MRUBY_ROOT}/../oniguruma/build"
    ld.libraries = ["ws2_32", "onig",]
  end

  conf.archiver do |ar|
    ar.command = "i686-w64-mingw32-ar"
  end

  conf.gembox "default"
  conf.gem :mgem => 'mruby-onig-regexp'
end
