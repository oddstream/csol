# make --recon

SRC_FILES = $(wildcard svg/Simple_*.svg)
OBJ_FILES = $(patsubst %.svg,%.png,$(SRC_FILES))

# target ... : prerequisites ...
#	recipe
#	...

# $@ The file name of the target of the rule
# $< The name of the first prequisite
# $* The stem with which an implicit rule matches

# Pattern rule for exporting svg to png
%.png: %.svg 
	inkscape $< --export-dpi=96 -o png/$(notdir $@)

all: $(OBJ_FILES)

check:
	@echo Source files: $(SRC_FILES)
	@echo Object files: $(OBJ_FILES)

clean:
	rm -f png/*.png