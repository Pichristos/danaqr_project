# DanaQR Makefile -- classic prc-tools (m68k-palmos-gcc) build.
#
# Prereqs on PATH: m68k-palmos-gcc, m68k-palmos-obj-res / build-prc,
# pilrc. See README.md for where to get each.
#
# This is written the way prc-tools projects have looked since the
# late 90s/early 2000s homebrew era -- adjust paths if your toolchain
# install differs.

VPATH = src

APPNAME   = DanaQR
CREATOR   = DnQR
CC        = m68k-palmos-gcc
CFLAGS    = -Wall -Os -palmos4

SRCS      = src/main.c src/chunker.c src/qr_render.c src/qrcode.c
OBJS      = $(SRCS:.c=.o)

RCP       = rsrc/DanaQR.rcp
RSRC_H    = src/DanaQR_rsrc.h

all: $(APPNAME).prc

# Regenerate the resource header + .bin resources from the .rcp.
# -H forces the emitted header name to match what main.c #includes.
rsrc-stamp: $(RCP)
	pilrc -H src/DanaQR_rsrc.h -o rsrc $(RCP)
	touch rsrc-stamp

%.o: %.c rsrc-stamp
	$(CC) $(CFLAGS) -Isrc -c $< -o $@

$(APPNAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(APPNAME) $(OBJS)

$(APPNAME).prc: $(APPNAME) rsrc-stamp
	build-prc -o $(APPNAME).prc -n "$(APPNAME)" -c $(CREATOR) $(APPNAME) rsrc/*.bin

clean:
	rm -f $(OBJS) $(APPNAME) $(APPNAME).prc rsrc-stamp rsrc/*.bin $(RSRC_H)

.PHONY: all clean
