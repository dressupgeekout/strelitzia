MRuby::Build.new do |conf|
  toolchain :gcc
  enable_debug

  conf.cc do |cc|
    cc.include_paths << "#{MRUBY_ROOT}/../oniguruma/src"
  end

  conf.linker do |ld|
    ld.link_options = %Q(%{flags} -o "%{outfile}" %{objs} %{libs})
    ld.library_paths << "#{MRUBY_ROOT}/../oniguruma/build"
    ld.libraries = ["onig",]
  end

  conf.gembox 'default'
  conf.gem :mgem => 'mruby-onig-regexp'
end
