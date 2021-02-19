/***********************************************************************
 
 Copyright 2006-2009 Ma Bingyao
 Copyright 2013 Gao Chunhui, Liu Tao
 
 These sources is free software. Redistributions of source code must
 retain the above copyright notice. Redistributions in binary form
 must reproduce the above copyright notice. You can redistribute it
 freely. You can use it with any free or commercial software.
 
 These sources is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY. Without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 
 github: https://github.com/liut/pecl-xxtea
 
 *************************************************************************/

#include "hero_xxtea.h"
#include <memory.h>
#include <stdlib.h>
#include "cocos2d.h"

using namespace std;
USING_NS_CC;

static void hero_xxtea_long_encrypt(xxtea_long *v, xxtea_long len, xxtea_long *k)
{
	CCLOG("hero_xxtea_long_encrypt in");
    xxtea_long n = len - 1;
    xxtea_long z = v[n], y = v[0], p, q = 6 + 52 / (n + 1), sum = 0, e;
    if (n < 1) {
        return;
    }
    while (0 < q--) {
        sum += XXTEA_DELTA;
        e = sum >> 2 & 3;
        for (p = 0; p < n; p++) {
            y = v[p + 1];
            z = v[p] += XXTEA_MX;
        }
        y = v[0];
        z = v[n] += XXTEA_MX;
    }
	CCLOG("hero_xxtea_long_encrypt out");
}

static void hero_xxtea_long_decrypt(xxtea_long *v, xxtea_long len, xxtea_long *k)
{
	CCLOG("hero_xxtea_long_decrypt in");
    xxtea_long n = len - 1;
    xxtea_long z = v[n], y = v[0], p, q = 6 + 52 / (n + 1), sum = q * XXTEA_DELTA, e;
    if (n < 1) {
        return;
    }
    while (sum != 0) {
        e = sum >> 2 & 3;
        for (p = n; p > 0; p--) {
            z = v[p - 1];
            y = v[p] -= XXTEA_MX;
        }
        z = v[n];
        y = v[0] -= XXTEA_MX;
        sum -= XXTEA_DELTA;
    }
	CCLOG("hero_xxtea_long_decrypt out");
}

static unsigned char *hero_fix_key_length(unsigned char *key, xxtea_long key_len)
{
	CCLOG("hero_fix_key_length in");
    unsigned char *tmp = (unsigned char *)malloc(16);
    memcpy(tmp, key, key_len);
    memset(tmp + key_len, '\0', 16 - key_len);
	CCLOG("hero_fix_key_length out");
    return tmp;
}

static xxtea_long *hero_xxtea_to_long_array(unsigned char *data, xxtea_long len, int include_length, xxtea_long *ret_len) {
    xxtea_long i, n, *result;
    CCLOG("hero_xxtea_to_long_array in");
    n = len >> 2;
    n = (((len & 3) == 0) ? n : n + 1);
    if (include_length) {
        result = (xxtea_long *)malloc((n + 1) << 2);
        result[n] = len;
        *ret_len = n + 1;
    } else {
        result = (xxtea_long *)malloc(n << 2);
        *ret_len = n;
    }
    memset(result, 0, n << 2);
    for (i = 0; i < len; i++) {
        result[i >> 2] |= (xxtea_long)data[i] << ((i & 3) << 3);
    }
    CCLOG("hero_xxtea_to_long_array out");
    return result;
}

static unsigned char *hero_xxtea_to_byte_array(xxtea_long *data, xxtea_long len, int include_length, xxtea_long *ret_len) {
    xxtea_long i, n, m;
    unsigned char *result;
    CCLOG("hero_xxtea_to_byte_array in");
    n = len << 2;
    if (include_length) {
        m = data[len - 1];
        if ((m < n - 7) || (m > n - 4)) return NULL;
        n = m;
    }
    result = (unsigned char *)malloc(n + 1);
    for (i = 0; i < n; i++) {
        result[i] = (unsigned char)((data[i >> 2] >> ((i & 3) << 3)) & 0xff);
    }
    result[n] = '\0';
    *ret_len = n;
    CCLOG("hero_xxtea_to_byte_array out");
    return result;
}

static unsigned char *hero_do_xxtea_encrypt(unsigned char *data, xxtea_long len, unsigned char *key, xxtea_long *ret_len) {
    unsigned char *result;	
    xxtea_long *v, *k, v_len, k_len;
    CCLOG("hero_do_xxtea_encrypt in");
    v = hero_xxtea_to_long_array(data, len, 1, &v_len);
    k = hero_xxtea_to_long_array(key, 16, 0, &k_len);
    hero_xxtea_long_encrypt(v, v_len, k);
    result = hero_xxtea_to_byte_array(v, v_len, 0, ret_len);
    free(v);
    free(k);
    CCLOG("hero_do_xxtea_encrypt out");
    return result;
}

static unsigned char *hero_do_xxtea_decrypt(unsigned char *data, xxtea_long len, unsigned char *key, xxtea_long *ret_len) {
    CCLOG("hero_do_xxtea_decrypt in");
	unsigned char *result;
    xxtea_long *v, *k, v_len, k_len;
    
    v = hero_xxtea_to_long_array(data, len, 0, &v_len);
    k = hero_xxtea_to_long_array(key, 16, 0, &k_len);
    hero_xxtea_long_decrypt(v, v_len, k);
    result = hero_xxtea_to_byte_array(v, v_len, 1, ret_len);
    free(v);
    free(k);
    CCLOG("hero_do_xxtea_decrypt out");
    return result;
}

unsigned char *hero_xxtea_encrypt(unsigned char *data, xxtea_long data_len, unsigned char *key, xxtea_long key_len, xxtea_long *ret_length)
{
	CCLOG("hero_xxtea_encrypt in");
    unsigned char *result;
    
    *ret_length = 0;
    
    if (key_len < 16) {
        unsigned char *key2 = hero_fix_key_length(key, key_len);
        result = hero_do_xxtea_encrypt(data, data_len, key2, ret_length);
        free(key2);
    }
    else
    {
        result = hero_do_xxtea_encrypt(data, data_len, key, ret_length);
    }
    CCLOG("hero_xxtea_encrypt out");
    return result;
}

unsigned char *hero_xxtea_decrypt(unsigned char *data, xxtea_long data_len, unsigned char *key, xxtea_long key_len, xxtea_long *ret_length)
{
    unsigned char *result;
    CCLOG("hero_xxtea_decrypt in");
    *ret_length = 0;
    
    if (key_len < 16) {
        unsigned char *key2 = hero_fix_key_length(key, key_len);
        result = hero_do_xxtea_decrypt(data, data_len, key2, ret_length);
        free(key2);
    }
    else
    {
        result = hero_do_xxtea_decrypt(data, data_len, key, ret_length);
    }
	CCLOG("hero_xxtea_decrypt out");
    return result;
}

/* }}} */
