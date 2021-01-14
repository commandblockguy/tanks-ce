# todo: seems to be calculating the list of C files prior to running convimg

NAME ?= TANKS
ICON ?= iconc.png
DESCRIPTION ?= "Tanks! CE"
COMPRESSED ?= YES
ARCHIVED ?= YES

OUTPUT_MAP ?= YES

CFLAGS ?= -Wall -Wextra -O3
CXXFLAGS ?= -Wall -Wextra -O3

# ----------------------------

ifndef CEDEV
$(error CEDEV environment path variable is not set)
endif

include $(CEDEV)/meta/makefile.mk

ifdef CEMSCRIPTEN
include $(CEMSCRIPTEN)/makefile.mk
endif

# ----------------------------

SHELL = bash
IMAGE_NAMES = {{{pl,en}_{base,turret},shell}_{0..8},mine_{0..2}}
CONVIMG_INPUT = $(shell echo src/data/gfx/trimmed/$(IMAGE_NAMES).png) src/data/gfx/tileset.png src/data/gfx/bg_tank.png src/data/gfx/fg_tank.png
CONVIMG_OUTPUT = $(shell echo src/data/gfx/$(IMAGE_NAMES).{c,h}) src/data/gfx/tileset.c src/data/gfx/tileset.h src/data/gfx/bg_tank.c src/data/gfx/bg_tank.h src/data/gfx/fg_tank.c src/data/gfx/fg_tank.h src/data/gfx/palette.c src/data/gfx/palette.h src/data/gfx/gfx.h src/data/gfx/enemy_palette.bin
BLENDER_OUTPUT = $(shell echo src/data/gfx/rendered/$(IMAGE_NAMES).png)
OFFSET_FILES = $(shell echo src/data/gfx/offsets/$(IMAGE_NAMES).h)

clean_gfx:
	rm -f src/data/gfx/*.{c,h,txt,bin}
	rm -rf src/data/gfx/{rendered,trimmed,offsets}

$(shell echo src/data/gfx/rendered/%_{0..8}.png) &: tank.blend
	mkdir -p src/data/gfx/rendered
	blender --background tank.blend --scene $* --engine BLENDER_EEVEE --render-output src/data/gfx/rendered/$*_#.png --render-anim

src/data/gfx/trimmed/%.png src/data/gfx/offsets/%.h &: src/data/gfx/rendered/%.png
	mkdir -p src/data/gfx/{trimmed,offsets}
	convert $< -background '#00ff00' -alpha remove -alpha off -fuzz 25% -trim -print "#define $*_offset_x %X\n#define $*_offset_y %Y\n" $@ | sed 's/\+//g' > src/data/gfx/offsets/$*.h

src/data/gfx/offsets/offsets.h: $(OFFSET_FILES)
	echo \#ifndef H_OFFSETS > src/data/gfx/offsets/offsets.h
	echo \#define H_OFFSETS >> src/data/gfx/offsets/offsets.h
	echo \#ifdef __cplusplus >> src/data/gfx/offsets/offsets.h
	echo extern \"C\" \{ >> src/data/gfx/offsets/offsets.h
	echo \#endif >> src/data/gfx/offsets/offsets.h
	for CURRENT_TYPE in $(IMAGE_NAMES); do \
		echo \#include \"$$CURRENT_TYPE.h\" ; \
	done >> src/data/gfx/offsets/offsets.h
	echo \#ifdef __cplusplus >> src/data/gfx/offsets/offsets.h
	echo \} >> src/data/gfx/offsets/offsets.h
	echo \#endif >> src/data/gfx/offsets/offsets.h
	echo \#endif >> src/data/gfx/offsets/offsets.h

src/data/gfx/enemy_pal.c src/data/gfx/enemy_pal.h &: src/data/gfx/enemy_palette.bin src/data/gfx/generate_enemy_palette.py
	cd src/data/gfx/ && python3 generate_enemy_palette.py


$(CONVIMG_OUTPUT) &: src/data/gfx/convimg.yaml $(CONVIMG_INPUT)
	cd src/data/gfx && convimg

all_gfx: $(CONVIMG_OUTPUT) src/data/gfx/offsets/offsets.h src/data/gfx/enemy_pal.c src/data/gfx/enemy_pal.h

.PHONY: tiles all_gfx clean_gfx
