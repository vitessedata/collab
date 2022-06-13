#ifndef XRG_H
#define XRG_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
#define XRG_EXTERN_BEGIN extern "C" {
#define XRG_EXTERN_END }
#else
#define XRG_EXTERN_BEGIN
#define XRG_EXTERN_END
#endif

XRG_EXTERN_BEGIN

const char *xrg_version();

#define XRG_INLINE inline __attribute__((always_inline))
#define XRG_MAGIC "XRG1"

/* item flags */
#define XRG_SHIFT_NULL 0
#define XRG_SHIFT_INVAL 1  // invalid or disqualified
#define XRG_SHIFT_EXCEPT 2 // overflow or underflow

#define XRG_FLAG_NULL (1 << XRG_SHIFT_NULL)     // is null
#define XRG_FLAG_INVAL (1 << XRG_SHIFT_INVAL)   // invalid
#define XRG_FLAG_EXCEPT (1 << XRG_SHIFT_EXCEPT) // exception

/* physical types */
#define XRG_PTYP_UNKNOWN 0
#define XRG_PTYP_INT8 1
#define XRG_PTYP_INT16 2
#define XRG_PTYP_INT32 3
#define XRG_PTYP_INT64 4
#define XRG_PTYP_INT128 5
#define XRG_PTYP_FP32 6
#define XRG_PTYP_FP64 7
#define XRG_PTYP_BYTEA 8
#define XRG_PTYP_MAX 8

/* logical types */
#define XRG_LTYP_UNKNOWN 0
#define XRG_LTYP_NONE 1
#define XRG_LTYP_STRING 2    /* must be PTYP_BYTEA */
#define XRG_LTYP_DECIMAL 3   /* must be INT64 or INT128 */
#define XRG_LTYP_INTERVAL 4  /* must be INT128 */
#define XRG_LTYP_TIME 5      /* must be INT64 */
#define XRG_LTYP_DATE 6      /* must be INT32 */
#define XRG_LTYP_TIMESTAMP 7 /* must be INT64 */
#define XRG_LTYP_MAX 7

/*  add support for these later
#define XRG_LTYP_JSON 8
#define XRG_LTYP_UUID 9
#define XRG_LTYP_MACADDR 10
#define XRG_LTYP_INET 11
*/

#define XRG_LTYP_PTYP(ltyp, ptyp) ((((int32_t)ltyp) << 16) | ptyp)

typedef struct xrg_interval_t xrg_interval_t;
struct xrg_interval_t {
  int64_t usec;
  int32_t day;
  int32_t mon;
};

/* Encode a raw bytea at src. Store the encoded version at dst.
   Returns the ptr to end of the encoded string. */
static XRG_INLINE char *xrg_bytea_encode(char *dst, const void *src, int len) {
  *(int32_t *)dst = len;
  memcpy(dst + 4, src, len);
  return dst + 4 + len;
}

/* Return the length of a decoded bytea */
static XRG_INLINE int32_t xrg_bytea_len(const void *encoded) {
  return *(int32_t *)encoded;
}

/* Return a ptr to byte# 0 of a decoded bytea */
static XRG_INLINE const char *xrg_bytea_ptr(const void *encoded) {
  return ((const char *)encoded) + 4;
}

/* Decode and print an encoded bytea; useful in debugging */
void xrg_bytea_print(const char *encoded);

typedef struct xrg_vechdr_t xrg_vechdr_t;
struct xrg_vechdr_t {
  /* header, 40 bytes */
  char magic[4];
  int16_t ptyp;      /* physical type */
  int16_t ltyp;      /* logical type */
  int16_t fieldidx;  /* starts from 0 */
  int16_t itemsz;    /* #byte per item */
  int16_t scale;     /* for decimal type */
  int16_t precision; /* for decimal type */
  int32_t nbyte;     /* uncompressed size of data[] */
  int32_t zbyte;     /* compressed size of data[]; when not compressed, zbyte ==
                        nbyte */
  int32_t nnull;     /* #items marked as null in vector */
  int32_t nitem;     /* number of items in vector */
  int32_t ninval;    /* #items marked as invalid in vector */
  int32_t unused1;
  int64_t unused2;
  /* body starts here */
  /* flag starts at &body[zbyte], for nitem bytes */
  /* padding starts at &body[zbyte + nitem]; */
};

/* ------------------------------------------------------------------
 *
 *   Vector Buffer
 *
 * ------------------------------------------------------------------ */
typedef struct xrg_vecbuf_t xrg_vecbuf_t;
struct xrg_vecbuf_t {
  xrg_vechdr_t header;
  char *data; /* array data[datatop] */
  int datatop;
  int datamax;
  char *flag; /* array flag[flagtop] */
  int flagtop;
  int flagmax;
};

/* Create a vector buffer.
 * Returns a vector buffer on success, NULL otherwise.
 */
xrg_vecbuf_t *xrg_vecbuf_create(int ptyp, int ltyp, int fieldidx, int scale,
                                int precision, char *errbuf, int errbuflen);

/* Destroy a vector buffer. */
void xrg_vecbuf_release(xrg_vecbuf_t *vbuf);

/* Append an array of int8 to the vector buffer. */
int xrg_vecbuf_append_int8(xrg_vecbuf_t *vbuf, int nitem, const int8_t *ptr,
                           const char *flag, char *errbuf, int errbuflen);

/* Append an array of int16 to the vector buffer. */
int xrg_vecbuf_append_int16(xrg_vecbuf_t *vbuf, int nitem, const int16_t *ptr,
                            const char *flag, char *errbuf, int errbuflen);

/* Append an array of int32 to the vector buffer. */
int xrg_vecbuf_append_int32(xrg_vecbuf_t *vbuf, int nitem, const int32_t *ptr,
                            const char *flag, char *errbuf, int errbuflen);

/* Append an array of int64 to the vector buffer. */
int xrg_vecbuf_append_int64(xrg_vecbuf_t *vbuf, int nitem, const int64_t *ptr,
                            const char *flag, char *errbuf, int errbuflen);

/* Append an array of int128 to the vector buffer. */
int xrg_vecbuf_append_int128(xrg_vecbuf_t *vbuf, int nitem,
                             const __int128_t *ptr, const char *flag,
                             char *errbuf, int errbuflen);

/* Append an array of fp32 to the vector buffer. */
int xrg_vecbuf_append_fp32(xrg_vecbuf_t *vbuf, int nitem, const float *ptr,
                           const char *flag, char *errbuf, int errbuflen);

/* Append an array of fp64 to the vector buffer. */
int xrg_vecbuf_append_fp64(xrg_vecbuf_t *vbuf, int nitem, const double *ptr,
                           const char *flag, char *errbuf, int errbuflen);

/* Append an array of byte arrays to the vector buffer. */
int xrg_vecbuf_append_bytea(xrg_vecbuf_t *vbuf, int nitem, char *const *ptr,
                            const int *len, char *flag, char *errbuf,
                            int errbuflen);

/* Extract byte arrays from a vector buffer. */
int xrg_vecbuf_extract_bytea(xrg_vecbuf_t *vbuf, int nitem, char **ptr,
                             int *len, char *errbuf, int errbuflen);

/* Align value */
static XRG_INLINE int xrg_align(int alignment, int value) {
  return (value + alignment - 1) & ~(alignment - 1);
}

/* Returns the serialized size of vbuf in #bytes on success, or -1 otherwise. */
static XRG_INLINE int xrg_vecbuf_size(xrg_vecbuf_t *vbuf) {
  int nb = sizeof(vbuf->header) + vbuf->datatop + vbuf->flagtop;
  return xrg_align(16, nb);
}

/* Audit a vecbuf. Returns true or false. */
int xrg_vecbuf_is_valid(const xrg_vecbuf_t *vbuf);

/* ------------------------------------------------------------------
 *
 *   Compress/ Uncompress
 *
 * ------------------------------------------------------------------ */

/* Check if the vbuf is compressed. Returns true or false. */
static XRG_INLINE int xrg_vecbuf_is_compressed(const xrg_vecbuf_t *vbuf) {
  return vbuf->header.nbyte != vbuf->header.zbyte;
}

/* Compress the buffer. This will modify vbuf->data pointer.
 * Returns 0 on success, -1 otherwise.
 * Note: the routine will not compress the buffer if it determines
 * that it is ineconomical to do so (e.g. when the buffer is less than 4KB in
 * length). Therefore, the buffer may or may not be compressed when this
 * function returns successfully.
 */
int xrg_vecbuf_compress(xrg_vecbuf_t *vbuf, char *errbuf, int errbuflen);

/* Uncompress the buffer. This will modify vbuf->data pointer.
 * Returns 0 on success, -1 otherwise.
 */
int xrg_vecbuf_uncompress(xrg_vecbuf_t *vbuf, char *errbuf, int errbuflen);

/* ------------------------------------------------------------------
 *
 *   Read / Write
 *
 * ------------------------------------------------------------------ */

/* Writes an array of vecbuf to disk to an xrg file.
 * Returns 0 on success, -1 otherwise.
 */
int xrg_file_create(const char *fname, int nvbuf, xrg_vecbuf_t **vbuf,
                    char *errbuf, int errbuflen);

/* Writes a vector to disk or network.
 * Returns 0 on success, -1 otherwise.
 * To find out how many bytes were written, call xrg_vecbuf_size(vbuf).
 */
int xrg_vecbuf_write(xrg_vecbuf_t *vbuf, int fd, char *errbuf, int errbuflen);

/* Reads a vector from disk (at current offset) or network.
 * Returns 0 on success, -1 otherwise.
 */
xrg_vecbuf_t *xrg_vecbuf_read(int fd, char *errbuf, int errbuflen);

/* Vector offsets in a file */
typedef struct xrg_vecoff_t xrg_vecoff_t;
struct xrg_vecoff_t {
  int nvec;
  int64_t offset[1];
};

/* Read the vector offsets from a file.
 * Returns 0 on success, -1 otherwise.
 */
xrg_vecoff_t *xrg_vecoff_from_file(int fd, char *errbuf, int errbuflen);

/* Free the previously allocated xrg_vecoff */
void xrg_vecoff_release(xrg_vecoff_t *vecoff);

/* ------------------------------------------------------------------
 *
 *   XRG Vector -- in memory serialized format
 *
 * ------------------------------------------------------------------ */
typedef struct xrg_vector_t xrg_vector_t;
struct xrg_vector_t {
  xrg_vechdr_t header;
  char data_starts_here;
};

/* Access the data[] in vec */
#define XRG_VECTOR_DATA(vec) (&((vec)->data_starts_here))

/* Access the flag[] in vec */
#define XRG_VECTOR_FLAG(vec) (XRG_VECTOR_DATA(vec) + (vec)->header.zbyte)

/* The serialized size of vec in #bytes */
#define XRG_VECTOR_RAWSIZE(vec)                                                \
  xrg_align(16,                                                                \
            sizeof((vec)->header) + (vec)->header.zbyte + (vec)->header.nitem)

/* Print */
int xrg_vector_print(FILE *fp, int nvec, const xrg_vector_t **vec, char delim,
                     const char *nullstr, char *errbuf, int errbuflen);

/* Compress vec in-place */
void xrg_vector_compress(xrg_vector_t *vec);

/* Uncompress vec. This always returns a new copy of vec. */
xrg_vector_t *xrg_vector_uncompress(const xrg_vector_t *vec, char *errbuf,
                                    int errbuflen);

/* Check if vec is compressed */
#define xrg_vector_is_compressed(vec)                                          \
  ((vec)->header.nbyte != (vec)->header.zbyte)

/* Read column idx from fname and return it */
xrg_vector_t *xrg_file_read(const char *fname, int idx, char *errbuf,
                            int errbuflen);

/* Read columns specified in idx[], and return them in vec[] */
int xrg_file_read_ex(const char *fname, int nidx, int *idx, xrg_vector_t **vec,
                     char *errbuf, int errbuflen);

/* Read vector at current file offset */
xrg_vector_t *xrg_vector_read(int fd, char *errbuf, int errbuflen);

/* Create a vector in memory (for fixed size content only) */
xrg_vector_t *xrg_vector_create(int nitem, int ptyp, int ltyp, int fieldidx,
                                int scale, int precision, char *errbuf,
                                int errbuflen);

/* Free an xrg_vector returned previously by routines in this lib */
void xrg_vector_release(xrg_vector_t *);

/* hash a vector. Result is stored in hash[].
   hash[] should be of size v->header.nitem, and should be
   initialized to 0xffffffff. */
int xrg_vector_hash(const xrg_vector_t *v, int nitem, uint32_t *hash,
                    char *errbuf, int errbuflen);

/* ------------------------------------------------------------------
 *
 *   Bloom filter
 *
 * ------------------------------------------------------------------ */

typedef struct xrg_bloom_t xrg_bloom_t;
struct xrg_bloom_t {
  uint8_t bmp[512];
};

/* Empty the bloom filer. */
void xrg_bloom_init(xrg_bloom_t *bloom);

/* Add data[] to bloom filter. */
void xrg_bloom_add(xrg_bloom_t *bloom, const void *data, int len);

/* Check if data[] passes the bloom filter. Returns 1 if passed, 0 otherwise. */
int xrg_bloom_isset(const xrg_bloom_t *bloom, const void *data, int len);

/* ------------------------------------------------------------------
 *
 *   Zonemap Writer
 *
 * ------------------------------------------------------------------ */

#define XRG_ZONEMAP_MAGIC "XRGZMAP2"

typedef struct xrg_zmwriter_t xrg_zmwriter_t;

/* Create a zonemap file. Returns 0 on success, -1 otherwise.
 */
xrg_zmwriter_t *xrg_zmwriter_open(const char *fname, int nfield,
                                  const char **cname, char *errbuf,
                                  int errbuflen);

/* Finish writing a zonemap file */
void xrg_zmwriter_close(xrg_zmwriter_t *);

/* Add a rowgroup to a zonemap file.
 * Note: the rowgroup must be compressed.
 */
int xrg_zmwriter_add(xrg_zmwriter_t *zmw, int32_t rgidx, xrg_vecbuf_t **vbuf,
                     char *errbuf, int errbuflen);

/* ------------------------------------------------------------------
 *
 *   Zonemap Reader
 *
 * ------------------------------------------------------------------ */
/* A Zonemap record. One per row-group per column-fragment. */
typedef struct xrg_zonerec_t xrg_zonerec_t;
struct xrg_zonerec_t {
  char magic[8];

  char cname[64]; /* max len is 63 */

  int32_t rgidx;    /* aka row-group id */
  int16_t nfield;   /* aka #column */
  int16_t fieldidx; /* aka column id */

  int16_t ptyp;
  int16_t ltyp;
  int16_t precision;
  int16_t scale;

  int16_t itemsz;
  char hasnull;
  char hasnonnull;
  char unused[4];

  char minval[128]; /* cutoff at 128 */
  char maxval[128]; /* cutoff at 128 */
  xrg_bloom_t bloom;
  char magic2[8];
};

typedef struct xrg_zonemap_t xrg_zonemap_t;

/* Open a zonemap file for reading */
xrg_zonemap_t *xrg_zonemap_open(const char *fname, char *errbuf, int errbuflen);

/* Close a zonemap file */
void xrg_zonemap_close(xrg_zonemap_t *);

/* Read a zonemap record identified by field index in row group rgidx. */
xrg_zonerec_t *xrg_zonemap_read(xrg_zonemap_t *zmap, int32_t rgidx,
                                int fieldidx, char *errbuf, int errbuflen);

/* -----------------------------------------
 * -- CRC32
 * ----------------------------------------- */
typedef uint32_t xrg_crc32c_t;

xrg_crc32c_t xrg_crc32c_init(const void *data, int len);
xrg_crc32c_t xrg_crc32c_mix(xrg_crc32c_t c, const void *data, int len);


XRG_EXTERN_END

#endif /* XRG_H */
