#include "qr_render.h"
#include "qrcode.h"

/* QRCodeGenPort: the caller provides the module buffer. We size it
 * for QR_MAX_VERSION at compile time so no malloc is needed at
 * runtime -- 68k stacks on Palm OS are small and heap fragmentation
 * is a real concern on a long-running applet.
 *
 * qrcode_getBufferSize() returns the byte count needed for a given
 * version's module grid (1 bit per module). For version 7 that's
 * (17 + 4*7)^2 / 8 = 45^2 / 8 = ~254 bytes. Round up generously. */
#define QR_MAX_MODULES_SQ   (17 + 4 * QR_MAX_VERSION) * (17 + 4 * QR_MAX_VERSION)
#define QR_BUF_BYTES        (((QR_MAX_MODULES_SQ + 7) / 8) + 16)

static qrcode_uint8_t sQrModules[QR_BUF_BYTES];

Boolean QRRenderChunk(const char *frameText, UInt16 frameLen,
                       const RectangleType *destRect)
{
    QRCode qr;
    Int16 size;
    Int16 side;
    Int16 scale;
    Int16 originX, originY;
    Int16 x, y;
    RectangleType modRect;

    (void)frameLen;

    if (!qrcode_initText(&qr, sQrModules, QR_MAX_VERSION,
                         QRCODE_ECC_LOW, frameText)) {
        return false;
    }

    size = (Int16)qr.size;
    if (size <= 0) {
        return false;
    }

    side = (Int16)((destRect->extent.x < destRect->extent.y)
                    ? destRect->extent.x : destRect->extent.y);
    scale = (Int16)(side / (size + 2));
    if (scale < 1) {
        scale = 1;
    }

    {
        Int16 drawnSide = (Int16)((size + 2) * scale);
        originX = (Int16)(destRect->topLeft.x
                    + (destRect->extent.x - drawnSide) / 2);
        originY = (Int16)(destRect->topLeft.y
                    + (destRect->extent.y - drawnSide) / 2);
    }

    /* Clear the QR area */
    WinEraseRectangle(destRect, 0);

    /* Draw modules directly to the screen */
    for (y = 0; y < size; y++) {
        for (x = 0; x < size; x++) {
            if (qrcode_getModule(&qr, (qrcode_uint8_t)x, (qrcode_uint8_t)y)) {
                modRect.topLeft.x = (Coord)(originX + (x + 1) * scale);
                modRect.topLeft.y = (Coord)(originY + (y + 1) * scale);
                modRect.extent.x = (Coord)scale;
                modRect.extent.y = (Coord)scale;
                WinPaintRectangle(&modRect, 0);
            }
        }
    }

    return true;
}