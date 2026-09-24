#ifndef DANAQR_ASSERT_H
#define DANAQR_ASSERT_H
/* Palm OS has no assert.h of its own -- no console/stderr for a
 * real assertion failure to report to. qrcodegen.c only uses
 * assert() for internal invariant checks (sizes fitting buffers,
 * etc.), which we've already sized correctly on our end (see
 * chunker.h's comments), so compiling these out entirely is safe
 * rather than trying to reimplement assert() against Palm APIs. */
#define assert(expr) ((void)0)
#endif
