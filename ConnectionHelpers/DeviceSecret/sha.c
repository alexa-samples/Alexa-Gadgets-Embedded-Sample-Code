#include <stdint.h>
#include <string.h>

/*
 *  FIPS-180-1 compliant SHA-1 implementation
 *
 *  Copyright (C) 2006-2007  Christophe Devine
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License, version 2.1 as published by the Free Software Foundation.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *  MA  02110-1301  USA
 *
 *  The SHA-1 standard was published by NIST in 1993.
 *
 *  http://www.itl.nist.gov/fipspubs/fip180-1.htm
 */

typedef struct
{
	uint32_t total[2];    /*!< number of bytes processed  */
	uint32_t state[5];    /*!< intermediate digest state  */
	uint8_t buffer[64];   /*!< data block being processed */

	uint8_t ipad[64];     /*!< HMAC: inner padding        */
	uint8_t opad[64];     /*!< HMAC: outer padding        */

} sha1_context;

/*
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef GET_ULONG_BE
#define GET_ULONG_BE(n,b,i)               \
{                                         \
	(n) = ((uint32_t) (b)[(i)    ] << 24)  \
	    | ((uint32_t) (b)[(i) + 1] << 16)  \
	    | ((uint32_t) (b)[(i) + 2] <<  8)  \
	    | ((uint32_t) (b)[(i) + 3]      ); \
}
#endif

#ifndef PUT_ULONG_BE
#define PUT_ULONG_BE(n,b,i)               \
{                                         \
	(b)[(i)    ] = (uint8_t) ((n) >> 24);  \
	(b)[(i) + 1] = (uint8_t) ((n) >> 16);  \
	(b)[(i) + 2] = (uint8_t) ((n) >>  8);  \
	(b)[(i) + 3] = (uint8_t) ((n)      );  \
}
#endif

/*
 * SHA-1 context setup
 */
static void sha1_starts (sha1_context *ctx)
{
	ctx->total[0] = 0;
	ctx->total[1] = 0;

	ctx->state[0] = 0x67452301;
	ctx->state[1] = 0xefcDab89;
	ctx->state[2] = 0x98badcfe;
	ctx->state[3] = 0x10325476;
	ctx->state[4] = 0xc3d2e1f0;
}

static void sha1_process (sha1_context *ctx, const uint8_t data[64])
{
	uint32_t temp, W[16], A, B, C, D, E;

	GET_ULONG_BE(W[ 0], data,  0);
	GET_ULONG_BE(W[ 1], data,  4);
	GET_ULONG_BE(W[ 2], data,  8);
	GET_ULONG_BE(W[ 3], data, 12);
	GET_ULONG_BE(W[ 4], data, 16);
	GET_ULONG_BE(W[ 5], data, 20);
	GET_ULONG_BE(W[ 6], data, 24);
	GET_ULONG_BE(W[ 7], data, 28);
	GET_ULONG_BE(W[ 8], data, 32);
	GET_ULONG_BE(W[ 9], data, 36);
	GET_ULONG_BE(W[10], data, 40);
	GET_ULONG_BE(W[11], data, 44);
	GET_ULONG_BE(W[12], data, 48);
	GET_ULONG_BE(W[13], data, 52);
	GET_ULONG_BE(W[14], data, 56);
	GET_ULONG_BE(W[15], data, 60);

#define S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

#define R(t)                                        \
(                                                   \
	temp = W[(t -  3) & 0x0F] ^ W[(t - 8) & 0x0F] ^  \
	       W[(t - 14) & 0x0F] ^ W[ t      & 0x0F],   \
          (W[t & 0x0F] = S(temp,1))                 \
)

#define P(a,b,c,d,e,x)                          \
{                                               \
	e += S(a,5) + F(b,c,d) + K + x; b = S(b,30); \
}

	A = ctx->state[0];
	B = ctx->state[1];
	C = ctx->state[2];
	D = ctx->state[3];
	E = ctx->state[4];

#define F(x,y,z) (z ^ (x & (y ^ z)))
#define K 0x5a827999

	P(A, B, C, D, E, W[0]);
	P(E, A, B, C, D, W[1]);
	P(D, E, A, B, C, W[2]);
	P(C, D, E, A, B, W[3]);
	P(B, C, D, E, A, W[4]);
	P(A, B, C, D, E, W[5]);
	P(E, A, B, C, D, W[6]);
	P(D, E, A, B, C, W[7]);
	P(C, D, E, A, B, W[8]);
	P(B, C, D, E, A, W[9]);
	P(A, B, C, D, E, W[10]);
	P(E, A, B, C, D, W[11]);
	P(D, E, A, B, C, W[12]);
	P(C, D, E, A, B, W[13]);
	P(B, C, D, E, A, W[14]);
	P(A, B, C, D, E, W[15]);
	P(E, A, B, C, D, R(16));
	P(D, E, A, B, C, R(17));
	P(C, D, E, A, B, R(18));
	P(B, C, D, E, A, R(19));

#undef K
#undef F

#define F(x,y,z) (x ^ y ^ z)
#define K 0x6ed9eba1

	P(A, B, C, D, E, R(20));
	P(E, A, B, C, D, R(21));
	P(D, E, A, B, C, R(22));
	P(C, D, E, A, B, R(23));
	P(B, C, D, E, A, R(24));
	P(A, B, C, D, E, R(25));
	P(E, A, B, C, D, R(26));
	P(D, E, A, B, C, R(27));
	P(C, D, E, A, B, R(28));
	P(B, C, D, E, A, R(29));
	P(A, B, C, D, E, R(30));
	P(E, A, B, C, D, R(31));
	P(D, E, A, B, C, R(32));
	P(C, D, E, A, B, R(33));
	P(B, C, D, E, A, R(34));
	P(A, B, C, D, E, R(35));
	P(E, A, B, C, D, R(36));
	P(D, E, A, B, C, R(37));
	P(C, D, E, A, B, R(38));
	P(B, C, D, E, A, R(39));

#undef K
#undef F

#define F(x,y,z) ((x & y) | (z & (x | y)))
#define K 0x8F1BBCDC

	P(A, B, C, D, E, R(40));
	P(E, A, B, C, D, R(41));
	P(D, E, A, B, C, R(42));
	P(C, D, E, A, B, R(43));
	P(B, C, D, E, A, R(44));
	P(A, B, C, D, E, R(45));
	P(E, A, B, C, D, R(46));
	P(D, E, A, B, C, R(47));
	P(C, D, E, A, B, R(48));
	P(B, C, D, E, A, R(49));
	P(A, B, C, D, E, R(50));
	P(E, A, B, C, D, R(51));
	P(D, E, A, B, C, R(52));
	P(C, D, E, A, B, R(53));
	P(B, C, D, E, A, R(54));
	P(A, B, C, D, E, R(55));
	P(E, A, B, C, D, R(56));
	P(D, E, A, B, C, R(57));
	P(C, D, E, A, B, R(58));
	P(B, C, D, E, A, R(59));

#undef K
#undef F

#define F(x,y,z) (x ^ y ^ z)
#define K 0xCA62C1D6

	P(A, B, C, D, E, R(60));
	P(E, A, B, C, D, R(61));
	P(D, E, A, B, C, R(62));
	P(C, D, E, A, B, R(63));
	P(B, C, D, E, A, R(64));
	P(A, B, C, D, E, R(65));
	P(E, A, B, C, D, R(66));
	P(D, E, A, B, C, R(67));
	P(C, D, E, A, B, R(68));
	P(B, C, D, E, A, R(69));
	P(A, B, C, D, E, R(70));
	P(E, A, B, C, D, R(71));
	P(D, E, A, B, C, R(72));
	P(C, D, E, A, B, R(73));
	P(B, C, D, E, A, R(74));
	P(A, B, C, D, E, R(75));
	P(E, A, B, C, D, R(76));
	P(D, E, A, B, C, R(77));
	P(C, D, E, A, B, R(78));
	P(B, C, D, E, A, R(79));

#undef K
#undef F

	ctx->state[0] += A;
	ctx->state[1] += B;
	ctx->state[2] += C;
	ctx->state[3] += D;
	ctx->state[4] += E;
}

/*
 * SHA-1 process buffer
 */
static void sha1_update (sha1_context *ctx, const uint8_t *src, int len)
{
	int fill;
	uint32_t left;

	if (len <= 0)
		return;

	left = ctx->total[0] & 0x3f;
	fill = 64 - left;

	ctx->total[0] += len;
	ctx->total[0] &= 0xffffffff;

	if (ctx->total[0] < (uint32_t) len)
		ctx->total[1]++;

	if (left && len >= fill)
	{
		memcpy (ctx->buffer + left, src, fill);

		sha1_process (ctx, ctx->buffer);

		src  += fill;
		len -= fill;
		left  = 0;
	}

	while (len >= 64)
	{
		sha1_process (ctx, src);

		src += 64;
		len -= 64;
	}

	if (len > 0)
		memcpy (ctx->buffer + left, src, len);
}

static const uint8_t sha1_padding[64] =
{
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * SHA-1 final digest
 */
static void sha1_finish (sha1_context *ctx, uint8_t output[20])
{
	uint32_t last, padn;
	uint32_t high, low;
	uint8_t msglen[8];

	high = (ctx->total[0] >> 29)
	     | (ctx->total[1] <<  3);
	low  = (ctx->total[0] <<  3);

	PUT_ULONG_BE(high, msglen, 0);
	PUT_ULONG_BE(low,  msglen, 4);

	last = ctx->total[0] & 0x3f;
	padn = (last < 56) ? (56 - last) : (120 - last);

	sha1_update(ctx, (uint8_t *) sha1_padding, padn);
	sha1_update(ctx, msglen, 8);

	PUT_ULONG_BE(ctx->state[0], output,  0);
	PUT_ULONG_BE(ctx->state[1], output,  4);
	PUT_ULONG_BE(ctx->state[2], output,  8);
	PUT_ULONG_BE(ctx->state[3], output, 12);
	PUT_ULONG_BE(ctx->state[4], output, 16);

	return;
}

/** Compute SHA1(160 bit) hash of a data block
 *
 * Computes the hash of a given block of data using
 * the FIPS-180-1 SHA1 implementation and a 160 bit
 * hash key
 *
 * @param output Address to receive the 20 byte SHA1 sum
 * @param data Block of data to compute hash on
 * @param len Length of data block
 */
void sha1sum (void *output, const void *data, int len)
{
	sha1_context ctx;

// --------------------------------------------------------------------

	sha1_starts (&ctx);
	sha1_update (&ctx, (uint8_t *) data, len);
	sha1_finish (&ctx, (uint8_t *) output);

	return;

} // end of "sha1sum ()"

/*
 * FIPS 180-2 SHA-224/256/384/512 implementation
 * Last update: 02/02/2007
 * Issue date:  04/30/2005
 *
 * Copyright (C) 2005, 2007 Olivier Gay <olivier.gay@a3.epfl.ch>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Modifications for tango2 and tango3, and inclusion in tango3 ROM
 * over the 2006-2009 period by em@sdesigns.com
 */

#define SHA_BLOCK_SIZE 64

typedef struct
{
	uint32_t sha256block[SHA_BLOCK_SIZE/4];
	uint32_t pos;
	uint64_t size;
	uint32_t digest[8];

} sha256_context;

static const uint32_t sha256_k__[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
	0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
	0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
	0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
	0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
	0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
};

#ifndef MIN
#define MIN(a,b)  ((a < b) ? a : b)
#endif

#define SHFR(x, n)    (x >> n)
#define ROTR(x, n)   ((x >> n) | (x << ((sizeof(x) << 3) - n)))
#define ROTL(x, n)   ((x << n) | (x >> ((sizeof(x) << 3) - n)))
#define CH(x, y, z)  ((x & y) ^ (~x & z))
#define MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))

#define HASIMBIT(x,log2im) ((int)(((uint32_t)(x)<<(31-(log2im)))) < 0)

#define SHA256_F1(x) (ROTR(x,  2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define SHA256_F2(x) (ROTR(x,  6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SHA256_F3(x) (ROTR(x,  7) ^ ROTR(x, 18) ^ SHFR(x,  3))
#define SHA256_F4(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ SHFR(x, 10))

static inline uint32_t BE2UINT32 (const uint8_t *buf)
{
	return ((uint32_t) buf[0] << 24)
	     | ((uint32_t) buf[1] << 16)
	     | ((uint32_t) buf[2] << 8)
	     |  (uint32_t) buf[3];
}

static void sha256_starts (sha256_context *ctx)
{
	ctx->digest[0] = 0x5be0cd19;
	ctx->digest[1] = 0x1f83d9ab;
	ctx->digest[2] = 0x9b05688c;
	ctx->digest[3] = 0x510e527f;
	ctx->digest[4] = 0xa54ff53a;
	ctx->digest[5] = 0x3c6ef372;
	ctx->digest[6] = 0xbb67ae85;
	ctx->digest[7] = 0x6a09e667;

	ctx->pos  = 0;
	ctx->size = 0;
}

static void sha256_process (sha256_context *ctx)
{
	int j;
	uint32_t a,b,c,d,e,f,g,h;
	uint32_t w[16];
	const uint32_t *sha256_k;
	
	a = ctx->digest[7];
	b = ctx->digest[6];
	c = ctx->digest[5];
	d = ctx->digest[4];
	e = ctx->digest[3];
	f = ctx->digest[2];
	g = ctx->digest[1];
	h = ctx->digest[0];

	sha256_k = sha256_k__;

	for (j = 0; !HASIMBIT(j,6); j++)
	{
		uint32_t t1,t2;
		int jc=j&0xf;
		
		if (j==jc) // e.g. j<16 
			w[j]=BE2UINT32((uint8_t*)(&(ctx->sha256block[j])));
		else 
			w[jc]+=SHA256_F4(w[(j-2)&0xf])+w[(j-7)&0xf]+SHA256_F3(w[(j-15)&0xf]);
		
		t1 = h + SHA256_F2(e) + CH(e, f, g)
		   + sha256_k[j] + w[jc];
		t2 = SHA256_F1(a) + MAJ(a, b, c);
		h  = g;
		g  = f;
		f  = e;
		e  = d + t1;
		d  = c;
		c  = b;
		b  = a;
		a  = t1 + t2;
	}

	ctx->digest[7] += a;
	ctx->digest[6] += b;
	ctx->digest[5] += c;
	ctx->digest[4] += d;
	ctx->digest[3] += e;
	ctx->digest[2] += f;
	ctx->digest[1] += g;
	ctx->digest[0] += h;
}

static void sha256_update (sha256_context *ctx, const uint8_t *src, int len)
{
	uint32_t pos = ctx->pos;
	uint8_t *dst = pos + (uint8_t *) ctx->sha256block;

	ctx->size += len;

	while (len > 0)
	{
		uint32_t cnt = MIN(SHA_BLOCK_SIZE - pos, (uint32_t) len);

		memcpy (dst, src, cnt);
		dst += cnt;
		pos += cnt;
		src += cnt;
		len -= cnt;
		
		if (pos == SHA_BLOCK_SIZE)
		{
			sha256_process (ctx);
			pos = 0;
			dst = (uint8_t *) ctx->sha256block;
		}
	}

	ctx->pos = pos;
}

static void sha256_finish (sha256_context *ctx, uint8_t output[32])
{
	uint32_t pos = ctx->pos;
	uint8_t *sha256block = (uint8_t *) ctx->sha256block;

	sha256block[pos] = 0x80;
	pos++;
	
	while (pos != SHA_BLOCK_SIZE - 8)
	{
		if (pos == SHA_BLOCK_SIZE)
		{
			sha256_process (ctx);

			pos = 0;
		}

		sha256block[pos++] = 0;
	}
	
	{
		uint64_t bitsize = ctx->size << 3;
		uint32_t bitsize_upper = bitsize >> 32;
		uint32_t bitsize_lower = (uint32_t) bitsize;

		ctx->sha256block[14] = BE2UINT32((uint8_t*) (&bitsize_upper));
		ctx->sha256block[15] = BE2UINT32((uint8_t*) (&bitsize_lower));
	}

	sha256_process (ctx);

	memcpy (output, ctx->digest, 8 * 4);

	return;
}

/** Compute SHA256 hash of a data block
 *
 * Computes the hash of a given block of data using
 * the FIPS-180-2 SHA1 implementation and a 256 bit
 * hash key
 *
 * @param output Address to receive the 32 byte SHA1 sum
 * @param data Block of data to compute hash on
 * @param len Length of data block
 */
void sha256sum (void *output, const void *data, int len)
{
	sha256_context ctx;

// --------------------------------------------------------------------

	sha256_starts (&ctx);
	sha256_update (&ctx, (uint8_t *) data, len);
	sha256_finish (&ctx, (uint8_t *) output);

	return;

} // end of "sha256sum ()"
