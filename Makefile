TOOLCHAIN=/opt/microchip/xc16/v1.70/bin/xc16

CFLAGS= \
	-O1 \
	-Wall \
	-mcpu=33EP512GM604 \

CC= $(TOOLCHAIN)-gcc
AS= $(TOOLCHAIN)-as

ASFLAGS= \
	-p 30F2010\

all:	minimal_example.objdump showfix

showfix:	minimal_example_fixed.objdump

clean:
	rm -f *.o
	rm -f *.objdump

%.objdump: %.o
	$(TOOLCHAIN)-objdump -S -s $< > $@

%.o: mplabx/xc16_int32_indexed_array_access.X/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

html: README.html

%.html: %.md
	cat misc/header > $@
	markdown < $< >> $@
	echo '</body>' >> $@

mplabx-zip:
	git clone . ,,z
	curdir=`pwd`; \
	cd ,,z/mplabx; \
	cp ../README.md ./xc16_int32_indexed_array_access.X; \
	zip -9 -r ../../mwe.zip .
	rm -rf ,,z

.PHONY: showfix all html zip

