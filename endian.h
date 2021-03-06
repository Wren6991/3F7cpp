
#ifndef _ENDIAN_H_CAMZIP_
#define _ENDIAN_H_CAMZIP_


#define ENDIAN_SWAP_32(x) ((((x) >> 24) & 0xff)| (((x) >> 8) & 0xff00) | (((x) << 8) & 0xff0000) | (((x) << 24) & 0xff000000))

#if defined(__LITTLE_ENDIAN) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)

#define be32_to_cpu(x) ENDIAN_SWAP_32(x)
#define le32_to_cpu(x) (x)

#elif defined(__BIG_ENDIAN) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)

#define be32_to_cpu(x) (x)
#define le32_to_cpu(x) ENDIAN_SWAP_32(x)

#else

#error "Neither __BIG_ENDIAN nor __LITTLE_ENDIAN defined. (Are you not using GCC?)"

#endif

#define cpu_to_le32(x) le32_to_cpu(x)
#define cpu_to_be32(x) be32_to_cpu(x)

#endif // _ENDIAN_H_CAMZIP_
