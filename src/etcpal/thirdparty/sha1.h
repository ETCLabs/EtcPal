#ifndef SHA1_H
#define SHA1_H

/*
   SHA-1 in C
   By Steve Reid <steve@edmweb.com>
   100% Public Domain
 */

/**************************************************************************************************
 * ETC made the following changes:
 * 05-30-2023 CGR
 *   - Switched names of SHA1_CTX struct and SHA1 functions to EtcPal-specific names to avoid conflicts
 *************************************************************************************************/

#include "stdint.h"

typedef struct
{
  uint32_t      state[5];
  uint32_t      count[2];
  unsigned char buffer[64];
} EtcPalSha1Ctx;

void etcpal_sha1_transform(uint32_t state[5], const unsigned char buffer[64]);

void etcpal_sha1_init(EtcPalSha1Ctx* context);

void etcpal_sha1_update(EtcPalSha1Ctx* context, const unsigned char* data, uint32_t len);

void etcpal_sha1_final(unsigned char digest[20], EtcPalSha1Ctx* context);

void etcpal_sha1(char* hash_out, const char* str, int len);

#endif /* SHA1_H */
