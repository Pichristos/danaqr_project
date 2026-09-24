#ifndef CHUNKER_H
#define CHUNKER_H

#include <PalmOS.h>

/* Max chars of PAYLOAD per QR frame, before the "Qnn/nn:" header is
 * added. Sized against QR_MAX_VERSION + ECC level in qr_render.h:
 * v10 at ECC LOW gives 213 bytes of byte-mode capacity; we subtract
 * 7 for the header and ~26 for encoder overhead/padding, leaving 180.
 * Tune this against whatever ECC level and version cap you pick in
 * qr_render.c -- the two files must agree. */
#define CHUNK_PAYLOAD_MAX   180

/* "Q" + 2 digit seq + "/" + 2 digit tot + ":" */
#define CHUNK_HEADER_LEN    7

#define CHUNK_FRAME_MAX     (CHUNK_HEADER_LEN + CHUNK_PAYLOAD_MAX)

/* Hard ceiling on chunk count (2-digit seq/tot in the header).
 * CHUNK_PAYLOAD_MAX * CHUNK_MAX_COUNT ~= 7.9 KB of source text,
 * which is a lot of typewriter output already. Bump the header to
 * 3 digits ("Q003/512:") if you need more. */
#define CHUNK_MAX_COUNT     99

typedef struct {
    char    text[CHUNK_FRAME_MAX + 1];   /* NUL-terminated frame, header+payload */
    UInt16  len;                          /* strlen(text), cached */
} ChunkType;

/* Splits srcText (srcLen bytes, not necessarily NUL-terminated) into
 * headered frames written to outChunks (caller-allocated array of at
 * least CHUNK_MAX_COUNT entries). Returns the number of chunks
 * produced, or 0 if srcLen is 0 or would exceed CHUNK_MAX_COUNT
 * chunks (caller should tell the user to shorten the text). */
UInt16 ChunkerBuildChunks(const char *srcText, UInt32 srcLen,
                           ChunkType *outChunks, UInt16 maxChunks);

#endif /* CHUNKER_H */
