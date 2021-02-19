
#pragma once

/// @cond DO_NOT_SHOW

#include "platform/CCPlatformMacros.h"

#ifdef __cplusplus
extern "C" {
#endif    
    
namespace cocos2d {

/** @file
 base64 helper functions
 */

/** 
 * Decodes a 64base encoded memory. The decoded memory is
 * expected to be freed by the caller by calling `free()`
 *
 * @returns the length of the out buffer
 *
 @since v0.8.1
 */
int CC_DLL nbase64Decode(const unsigned char *in, unsigned int inLength, unsigned char **out);
    
/**
 * Encodes bytes into a 64base encoded memory with terminating '\0' character. 
 * The encoded memory is expected to be freed by the caller by calling `free()`
 *
 * @returns the length of the out buffer
 *
 @since v2.1.4
 */
int CC_DLL nbase64Encode(const unsigned char *in, unsigned int inLength, char **out);

}//namespace   cocos2d 

#ifdef __cplusplus
}
#endif    

/// @endcond
