#ifndef _SHA1_H_
#define _SHA1_H_

#include <unistd.h>
#include <sys/types.h>

#include "transfer.h"

#define SHA1SIZE 128
#define SHA1CircularShift(bits,word) ((((word) << (bits)) & 0xFFFFFFFF) | ((word) >> (32-(bits))))

typedef struct SHA1Context{
    unsigned Message_Digest[5];      
    unsigned Length_Low;             
    unsigned Length_High;            
    unsigned char Message_Block[64]; 
    int Message_Block_Index;         
    int Computed;                    
    int Corrupted;                   
} SHA1Context;

int SHA1Result ( SHA1Context* );

void SHA1Reset ( SHA1Context* );
void SHA1Input ( SHA1Context*, const char*, unsigned );

void SHA1PadMessage ( SHA1Context* );
void SHA1ProcessMessageBlock ( SHA1Context* );

/********************************************************************************************************************
 * @desc    生成给定内存块的sha1验证码
 * @param   void*   source      需要生成验证信息的内存块指针
 * @param   size_t  length      内存块的长度
 * @param   char*   sha1str     保存验证字符串的内存指针，至少要为SHA1SIZE个字节大小
 * @return  bool                成功生成验证字符串，返回true，否则，返回false
 ********************************************************************************************************************/
bool sha1_hash ( const void* source, const size_t length, char* sha1str );

#endif
