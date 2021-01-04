RUBY?=		ruby
BUNDLE?=	bundle

RENDER_TOOL:=	render.rb
TEMPLATE:=	layout_default.erb

ARTICLES=	# defined
ARTICLES+=	html/main.html
ARTICLES+=	html/engine.html

.PHONY: all
all: $(ARTICLES)

html/%.html: articles/%.md $(RENDER_TOOL) $(TEMPLATE) | html vendor
	$(BUNDLE) exec $(RUBY) $(RENDER_TOOL) --template $(TEMPLATE) --article $< > $@

vendor:
	$(BUNDLE) install --path $@

html:
	mkdir -p $@

.PHONY: clean
clean:
	rm -rf $(ARTICLES) html
