ALLPNG= \
	gen/assp_simp.png \
	gen/assp_opt.png

ALLDOT= \
	gen/api.png \
	gen/assp.png

PUBLISH= \
	assp_apiref.html \
	assp_compsctp.html \
	assp_comptcp.html \
	assp_impl.html \
	assp_main.html \
	assp_packet_format.html \
	assp_rationale.html \
	assp_req.html \
	assp_states.html \
	gen/api.png \
	gen/assp.png \
	gen/assp_opt.png \
	gen/assp_simp.png \
	retrans.html

all: $(ALLPNG) $(ALLDOT)

gen/%.png: gen/%.ps gen/.exists errs/gen/.exists
	gs -sDEVICE=pngmono -r200x200 -sOutputFile=$@ $< </dev/null

gen/%.png: gen/%_l.ps gen/.exists errs/gen/.exists
	gs -g3300x2400 -sDEVICE=pngmono -r300x300 -sOutputFile=$@ $< </dev/null

gen/%.ps: %.pic gen/.exists errs/gen/.exists
	groff -R -p -Tps $< >$@

$(ALLDOT): gen/%.png: %.dot gen/.exists errs/gen/.exists
	dot -Tpng -o $@ $<

install: $(PUBLISH)
	for i in $(PUBLISH); do cp $$i ~/public_html/Px_kjasdfh/proto/$$i; done

%/.exists:
	mkdir -p $(dir $@)
	:>$@
