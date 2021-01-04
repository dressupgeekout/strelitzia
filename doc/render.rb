require 'erb'
require 'optparse'

require 'kramdown'

template = nil
article = nil

parser = OptionParser.new do |opts|
  opts.on("--template PATH") { |path| template = File.expand_path(path) }
  opts.on("--article PATH") { |path| article = File.expand_path(path) }
end

parser.parse!(ARGV)

if !template || !article
  $stderr.puts(parser.to_s)
  exit 1
end

$content = Kramdown::Document.new(File.read(article)).to_html
puts ERB.new(File.read(template)).result(binding)
