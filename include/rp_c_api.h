#if 0
#ifndef RP_C_API_H
#define RP_C_API_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float x, y, w, h;
} RpRect;

typedef struct {
    int src, dst;
} RpIndexPair;

typedef struct {
    float x, y;
} RpPosition;

/*
 * Packs the rectangles into the smallest possible area.
 * @param out_positions The output positions of the rectangles.
 * @param rects The rectangles to pack.
 * @param n_rects The number of rectangles.
 * @param indices The parent-child indices.
 * @param n_indices The number of indices.
 * @return The number of packed rectangles. -1 if an error occurred.
 * For error details, call rp_get_error().
 */
int rp_pack_rects(RpPosition* out_positions,
                  RpRect* rects,
                  int n_rects,
                  RpIndexPair* indices,
                  int n_indices,
                  RpPosition center);

/*
 * Gets the last error message.
 * @return The last error message.
 */
const char* rp_get_error();

int rp_version();

#ifdef __cplusplus
} // extern "C"
#endif

#endif // RP_C_API_H
#endif // 0
