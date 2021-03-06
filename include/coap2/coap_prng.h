/*
 * prng.h -- Pseudo Random Numbers
 *
 * Copyright (C) 2010-2011 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see README for terms
 * of use.
 */

/**
 * @file prng.h
 * @brief Pseudo Random Numbers
 */

#ifndef COAP_PRNG_H_
#define COAP_PRNG_H_

/**
 * @defgroup prng Pseudo Random Numbers
 * API functions for gerating pseudo random numbers
 * @{
 */

#if defined(WITH_CONTIKI)
#include <string.h>

/**
 * Fills \p buf with \p len random bytes. This is the default implementation for
 * prng(). You might want to change prng() to use a better PRNG on your specific
 * platform.
 */
COAP_STATIC_INLINE int
contiki_prng_impl(unsigned char *buf, size_t len) {
  uint16_t v = random_rand();
  while (len > sizeof(v)) {
    memcpy(buf, &v, sizeof(v));
    len -= sizeof(v);
    buf += sizeof(v);
    v = random_rand();
  }

  memcpy(buf, &v, len);
  return 1;
}

#define prng(Buf,Length) contiki_prng_impl((Buf), (Length))
#define prng_init(Value) random_init((uint16_t)(Value))
#elif defined(WITH_LWIP) && defined(LWIP_RAND)
COAP_STATIC_INLINE int
lwip_prng_impl(unsigned char *buf, size_t len) {
  u32_t v = LWIP_RAND();
  while (len > sizeof(v)) {
    memcpy(buf, &v, sizeof(v));
    len -= sizeof(v);
    buf += sizeof(v);
    v = LWIP_RAND();
  }

  memcpy(buf, &v, len);
  return 1;
}

#define prng(Buf,Length) lwip_prng_impl((Buf), (Length))
#define prng_init(Value)
#elif defined(_WIN32)
#define prng_init(Value)
errno_t __cdecl rand_s( _Out_ unsigned int* _RandomValue );
 /**
 * Fills \p buf with \p len random bytes. This is the default implementation for
 * prng(). You might want to change prng() to use a better PRNG on your specific
 * platform.
 */
COAP_STATIC_INLINE int
coap_prng_impl( unsigned char *buf, size_t len ) {
        while ( len != 0 ) {
                uint32_t r = 0;
                size_t i;
                if ( rand_s( &r ) != 0 )
                        return 0;
                for ( i = 0; i < len && i < 4; i++ ) {
                        *buf++ = (uint8_t)r;
                        r >>= 8;
                }
                len -= i;
        }
        return 1;
}

#else

/**
 * Data type for random number generator function. The function must
 * fill @p len bytes of random data into the buffer starting at @p
 * out.  On success, the function should return 1, zero otherwise.
 */
typedef int (*coap_rand_func_t)(void *out, size_t len);

/**
 * Replaces the current random number generation function with the
 * default function @p rng.
 *
 * @param rng  The random number generation function to use.
 */
void coap_set_prng(coap_rand_func_t rng);

/**
 * Seeds the default random number generation function with the given
 * @p seed. The default random number generation function will use
 * getrandom() if available, ignoring the seed.
 *
 * @param seed  The seed for the pseudo random number generator.
 */
void coap_prng_init(unsigned long seed);

/**
 * Fills @p buf with @p len random bytes using the default pseudo
 * random number generator. The default PRNG can be changed with
 * coap_set_prng(). This function returns 1 when @p len random bytes
 * have been written to @buf, zero otherwise.
 *
 * @param buf  The buffer to fill with random bytes.
 * @param len  The number of random bytes to write into @p buf.
 *
 * @param 1 on success, 0 otherwise.
 */
int coap_prng(void *buf, size_t len);

#ifndef prng
/**
 * Fills \p Buf with \p Length bytes of random data.
 *
 * @deprecated Use coap_prng() instead.
 * @hideinitializer
 */
#define prng(Buf,Length) coap_prng((Buf), (Length))
#endif

#ifndef prng_init
/**
 * Called to set the PRNG seed. You may want to re-define this to allow for a
 * better PRNG.
 *
 * @deprecated Use coap_prng_init() instead.
 * @hideinitializer
 */
#define prng_init(Value) coap_prng_init((unsigned long)(Value))
#endif

#endif /* POSIX */

/** @} */

#endif /* COAP_PRNG_H_ */
