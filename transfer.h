#ifndef TRANSFER_H
#define TRANSFER_H

#include <math.h>
#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <netdb.h> 
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define TRANSFER_DEBUG
#undef TRANSFER_DEBUG

/**测试使用的文件的名称**/
#define CFILE_BINARY "/Users/zhonglei/Pictures/joy.jpg" 
#define CFILE_TEXT "/Users/zhonglei/Downloads/index.html"

/**传输的日志文件**/
#define LOG_FILE "./logs/"

/**传输报文的最小大小**/
#define GRAM_MIN_SIZE (sizeof(Gram)-BUFFER_SIZE)

/**常用的各种数字进制的基数大小**/
#define HEX_BASE 16
#define DEC_BASE 10
#define OCT_BASE 8
#define BIN_BASE 2

/**对指针，打开的文件进行测试**/
#define ASSERTP(p) if(p==NULL){transfererr("POINTER USING WHEN NOT INITIALIZED....",TMSG_E);}
#define ASSERTF(f) if(f==NULL){transfererr("FILE HANDLER USING WHEN NOT INITIALIZED, MAYBE FILE NOT EXIST....",TMSG_E);}

/***************************************************************************************************************************************
 ** void* memcpy(void *restrict dst, const void *restrict src, size_t n);
 ** The memcpy() function returns the original value of dst.
 ** The memcpy() function copies n bytes from memory area src to memory area dst.  
 ** If dst and src overlap, behavior is undefined.  Applications in which dst and src might overlap should use memmove(3) instead. 
 ***************************************************************************************************************************************/
#define TMSG_STD
#undef  TMSG_STD

#define TMSG_LEVEL 	2

#define TMSG_LVL_D 	1
#define TMSG_LVL_L  2
#define TMSG_LVL_N 	3
#define TMSG_LVL_W	4
#define TMSG_LVL_E 	5
#define TMSG_LVL_C	6
#define TMSG_LVL_F  7

#define TMSG_D 1 
#define TMSG_L 2 
#define TMSG_N 3 
#define TMSG_W 4 
#define TMSG_E 5 
#define TMSG_C 6 
#define TMSG_F 7 
// TMSG_D/*DEBUG*/, TMSG_L/*LOG*/ TMSG_N/*NOTICE*/, TMSG_W/*WARNING*/, TMSG_E/*ERROR*/, TMSG_C/*CRITICAL*/, TMSG_F/*FETAL*/

typedef int TRANSMSG ; 
typedef enum { TRANSFER_SENDER, TRANSFER_RECEIVER } TransRole ; 
typedef enum { GRAM_NULL, GRAM_NIL=111, GRAM_DATA, GRAM_LENGTH, GRAM_FILENAME, GRAM_FEEDBACK } GramType;
typedef enum { false=0, true } bool;

const static unsigned int DEFAULT_PORT   = 8001;
const static char         DEFAULT_HOST[] = "127.0.0.1"; // "168.0.0.98";

#define MAX_FILENAME 1024
#define MAX_PATHNAME 896
#define MAX_NAME_LEN 128
#define NLEN 99999999999999999

const static char   NIL[]      = "NIL"; 
const static char   FEEDBACK[] = "FEEDBACK";      

typedef struct {
	long  length;
	void* pcontent;
} FCONTENT ;

#define MAX_LOG_MSG 2048
static char GLOBAL_LOGMSG[MAX_LOG_MSG];

/***************************************************************************************************************************************
 ** @desc    处理传输的错误信息
 ** @param   char*      msg     错误消息
 ** @return  char*                   保存文件内容的数据区域指针 
 ***************************************************************************************************************************************/
#define transfererror(a) transfererr ( (a), TMSG_D ) 
void transfererr ( const char* msg, const TRANSMSG code );
void transfermsg ( const char* msg, const TRANSMSG level );

/***************************************************************************************************************************************
 ** @desc	根据指定的名称，创建文件，若指定文件名称中文件名包含的目录不存在，则递归创建指定的目录，然后创建名称为指定文件名的空文件
 ** @param 	char* 	filename 		指定的文件名
 ** @return bool					操作的结果，若创建成功，返回true，否则，返回false
 **************************************************************************************************************************************/
bool makefile ( const char* filename );

/***************************************************************************************************************************************
 ** @desc    将指定文件的内容读取到指定的缓存区域中
 ** @param   char*      filename     文件的名称
 ** @return  FCONTENT                包含文件长度和文件内容数据区域指针的结构体 
 ***************************************************************************************************************************************/
FCONTENT readfile ( const char* filename );

/***************************************************************************************************************************************
 ** @desc    获取制定文件名的文件的长度
 ** @param   char*      filename     文件的名称
 ** @return  long                    若文件存在返回文件的长度，否则，返回－1 
 ***************************************************************************************************************************************/
size_t filelength ( const char* filename );

/***************************************************************************************************************************************
 ** @return 	获取指定文件的文件名（不含路径名）
 ** @param 		char* 	filename 		给定文件名称的字符串
 ** @param 		char* 	absname 		保存结果的字符串指针，调用方需确保该指针空间大小至少为MAX_NAME_LEN大小
 ** @return 	size_t					文件绝对名称的长度
 ***************************************************************************************************************************************/
int basename ( const char* filename, char* basename );

/***************************************************************************************************************************************
 ** @return 	获取指定文件或目录所在路径的名称
 ** @param 		char* 	filename 		给定文件名称的字符串
 ** @param 		char* 	absname 		保存结果的字符串指针，调用方需确保该指针空间大小至少为MAX_FILENAME大小
 ** @return 	size_t					文件或目录路径名的长度
 ***************************************************************************************************************************************/
int pathname ( const char* filename, char* pathname );

/***************************************************************************************************************************************
 ** @return 	获取指定文件名的绝对路径名
 ** @param 		char* 	filename 		给定文件名称的字符串
 ** @param 		char* 	absname 		保存结果的字符串指针，调用方需确保该指针空间大小至少为MAX_FILENAME大小
 ** @return 	size_t					文件绝对名称的长度
 ***************************************************************************************************************************************/
int absname ( const char* filename, char* absname );

/***************************************************************************************************************************************
 ** @return 	获取指定文件名的文件后缀名(含"."字符)
 ** @param 		char* 	filename 		给定文件名称的字符串
 ** @param 		char* 	absname 		保存结果的字符串指针，调用方需确保该指针空间大小至少为MAX_NAME_LEN大小
 ** @return 	size_t					文件绝对名称的长度
 ***************************************************************************************************************************************/
int extname ( const char* filename, char* extname );

/***************************************************************************************************************************************
 ** @desc    从指针指定的位置开始，写入指定指定字节数的内容至指定名称的文件中, 若文件存在，首先将文件长度截短为0
 ** @param 	 void* 		content 	 需要写入的数据的缓存区
 ** @param   size_t     length 		 需要写入的数据的长度
 ** @param   char*      filename     文件的名称
 ** @return  bool                    返回写入文件的字节数
 ***************************************************************************************************************************************/
size_t store ( const void* content, size_t length, const char* filename );

/***************************************************************************************************************************************
 ** @desc    结合当前时间戳，返回一个随机数字
 ** @return  unsigned long                   返回的随机数 
 ***************************************************************************************************************************************/
unsigned long trandom ();

/***************************************************************************************************************************************
 ** @desc    返回当前的时间戳
 ** @return  unsigned long                   当前时间戳 
 ***************************************************************************************************************************************/
unsigned long timestamp ();

/***************************************************************************************************************************************
 ** @desc	 将指定内存区域的值以十六进制字符串的形式输出到指定的存储区域
 ** @param 	 void* 		value 	需要输出的内存区域的指针
 ** @param   size)t 	length 	输出的内存区域的长度
 ** @param 	 char* 		store 	保存输出字符串的内存区域
 **************************************************************************************************************************************/
 size_t hexstr ( void* value, size_t length, char* store );

/***********************************************************************************************************************************
 ** @desc 快速排序算法在指定的数量级上的性能表现； 左侧为待排序数组的数量级，右侧为当前快速排序实现的执行时间，时间精度为秒
 ** 0 => 0
 ** 10 => 0
 ** 100 => 0
 ** 1000 => 0
 ** 10000 => 0
 ** 100000 => 0
 ** 1000000 => 0
 ** 10000000 => 11
 ** 100000000 => ? ： 内存受限
***********************************************************************************************************************************/
void quicksort ( int* array, int count ); 

/***************************************************************************************************************************************
 ** @desc	 冒泡排序
 ** @param 	 int* 		array 	需要排序的数组指针
 ** @param   int 		count 	数组的元素的个数
 **************************************************************************************************************************************/
void bubblesort ( int* array, int count );

#endif
