# DanaQR — text-out-via-QR for AlphaSmart Dana (PalmOS 4.1)

A minimal PalmOS app skeleton that takes text typed/pasted into a field,
splits it into chunks, and cycles through QR codes on screen so a phone
camera can reassemble the original text. This is a starting skeleton,
not a compiled/tested binary — the PalmOS 68k toolchain isn't available
in this environment, so treat this as "90% there, needs a build pass
on real tools" rather than drop-in-ready.

## Layout

```
dana-qr-export/
  src/
    main.c        - app shell: form, event loop, timer-driven cycling
    chunker.c/.h   - splits text into QR-sized, headered chunks
    qr_render.c/.h - draws a qrcodegen_Buffer onto the Palm screen
    qrcodegen.h    - NOT INCLUDED, see "QR library" below
    qrcodegen.c    - NOT INCLUDED, see "QR library" below
  rsrc/
    DanaQR.rcp     - PilRC resource script (form, field, buttons)
  Makefile
```

## QR library

I did not vendor Project Nayuki's `qrcodegen` C library into this
skeleton — it's a substantial third-party project and I'd rather you
pull the real, current source than get a hand-typed reconstruction
from me. It's MIT-licensed, single .c/.h pair, no dependencies, and
is exactly the right size for a constrained 68k device:

    https://github.com/nayuki/QR-Code-generator  (C version, in c/ folder)

Drop `qrcodegen.c` and `qrcodegen.h` into `src/`. The API this skeleton
assumes (matches upstream):

```c
bool qrcodegen_encodeText(const char *text, uint8_t tempBuffer[],
    uint8_t qrcode[], enum qrcodegen_Ecc ecl, int minVersion, int maxVersion,
    enum qrcodegen_Mask mask, bool boostEcl);
int  qrcodegen_getSize(const uint8_t qrcode[]);
bool qrcodegen_getModule(const uint8_t qrcode[], int x, int y);
```

You'll want `qrcodegen_BUFFER_LEN_FOR_VERSION(N)` sized buffers — see
the library's header for the macro, and pick N so `QRCODEGEN_VERSION`
covers your chunk payload size (see chunker.h comments).

## Screen strategy

The Dana's panel is 560x160, but a plain PalmOS 4 app only owns the
standard 160x160 "silkscreen" area unless it uses AlphaSmart's Dana
hi-res extension (a small addendum SDK AlphaSmart shipped separately,
not part of the stock Palm OS 5 headers). This skeleton deliberately
targets the safe 160x160 region so it runs unmodified on the Dana
exactly as any other Palm OS 4 app would — no Dana-specific extension
required. If you want to use the full 560-pixel width later (bigger
modules = more reliable camera reads, or bigger QR versions = fewer
chunks), that's a follow-on enhancement, not a rewrite: only
`qr_render.c`'s destination rectangle needs to change.

## Chunk protocol

Each QR encodes a small text frame, not raw payload:

```
Q<seq2>/<tot2>:<payload>
```

e.g. `Q03/12:the quick brown fox...`. `seq`/`tot` are zero-padded
2-digit indices (max 99 chunks — plenty for typewriter-length text;
bump to 3 digits in chunker.c if you need more). The receiving phone
app (or a script you write) collects frames by `tot`, sorts by `seq`,
strips the header, and concatenates. There's no error-correction
*across* frames — QR's own Reed-Solomon handles per-frame corruption,
but a dropped/missed frame just means you wait for the cycle to loop
back around, since cycling is continuous, not one-shot.

## Build (once you have the toolchain)

Classic homebrew Palm OS stack, still functional:

1. **prc-tools** (gcc-based m68k-palmos cross compiler) — the Dana's
   Dragonball VZ is a 68k-family core, not ARM, so this is the right
   toolchain despite its age.
2. **pilrc** to compile `DanaQR.rcp` into a resource-bearing `.prc`.
3. **Palm OS SDK headers** (SDK 5 headers work fine for OS 4.1 target
   apps — just don't call OS 5-only APIs).

```
make            # produces DanaQR.prc
```

Test in the Palm OS Emulator (POSE) with a Dana ROM/skin before
loading onto real hardware — much faster iteration loop, and you can
literally photograph your monitor to test the QR reading path.

## Known rough edges / TODO

- `qr_render.c` uses `WinPaintRectangle` for module fill, which is
  the standard OS 3.5+ paint call — double check against your SDK
  header version if you hit a link error, older SDKs sometimes only
  expose `WinDrawRectangle` with a fill pattern set via `WinSetPattern`.
- No persistence yet: the input field's contents are what you type
  each session. Wiring it up to read an AlphaWord `.pdb` or a Memo Pad
  record instead of manual paste is the natural next step once this
  compiles.
- Chunk cycle interval (`gCycleMs` in main.c) is a guess — 1.5s per
  frame is a reasonable starting point for a phone scanning a
  grayscale LCD, but tune it against your actual capture app.
