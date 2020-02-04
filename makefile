
NAME        ?= TANKS
COMPRESSED  ?= YES
ICONSRC     ?= iconc.png
DESCRIPTION ?= "Tanks! CE"

# ----------------------------

include $(CEDEV)/include/.makefile

# ----------------------------

tiles:
	@$(MAKE) -C src/tiles --no-print-directory

.PHONY: tiles