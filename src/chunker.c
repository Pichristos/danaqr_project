#include "chunker.h"

/* Simple 2-digit zero-padded decimal writer, val must be 0..99. */
static void WriteTwoDigits(char *dst, UInt16 val)
{
    dst[0] = (char)('0' + (val / 10) % 10);
    dst[1] = (char)('0' + (val % 10));
}

UInt16 ChunkerBuildChunks(const char *srcText, UInt32 srcLen,
                           ChunkType *outChunks, UInt16 maxChunks)
{
    UInt16 total;
    UInt16 i;
    UInt32 offset;

    if (srcLen == 0 || srcText == NULL || outChunks == NULL) {
        return 0;
    }

    total = (UInt16)((srcLen + CHUNK_PAYLOAD_MAX - 1) / CHUNK_PAYLOAD_MAX);
    if (total == 0) {
        total = 1;
    }
    if (total > CHUNK_MAX_COUNT || total > maxChunks) {
        /* Caller should surface an alert: "text too long, shorten or
         * raise CHUNK_PAYLOAD_MAX / header digits". We fail closed
         * rather than silently truncating the user's text. */
        return 0;
    }

    offset = 0;
    for (i = 0; i < total; i++) {
        UInt32 remaining = srcLen - offset;
        UInt16 thisLen = (UInt16)(remaining < CHUNK_PAYLOAD_MAX
                                    ? remaining : CHUNK_PAYLOAD_MAX);
        char *p = outChunks[i].text;

        *p++ = 'Q';
        WriteTwoDigits(p, (UInt16)(i + 1));
        p += 2;
        *p++ = '/';
        WriteTwoDigits(p, total);
        p += 2;
        *p++ = ':';

        MemMove(p, (void *)(srcText + offset), thisLen);
        p += thisLen;
        *p = '\0';

        outChunks[i].len = (UInt16)(p - outChunks[i].text);
        offset += thisLen;
    }

    return total;
}
