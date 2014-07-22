#include "gram.h"

bool
feedback ( const Gram* src, Gram* feedback ) {
	feedback->_source = src->_source;
	feedback->_length = src->_length;
	feedback->_type   = GRAM_FEEDBACK;
	feedback->_task   = src->_task;
	feedback->_num    = src->_num;
	feedback->_cnt    = src->_cnt;
	feedback->_len    = SHA1SIZE;

	if ( ! sha1_hash ( (void*)src->_data, src->_len, feedback->_data ) ) return false ; else 
	return true;
}

bool
first ( const void* data, const size_t length, const char* storename, Gram* first ) {
	size_t count = length/BUFFER_SIZE;
	size_t left  = length%BUFFER_SIZE;
	if ( left>0 ) count++;
	// unsigned long count = (unsigned long)ceil((double)length/BUFFER_SIZE);
	first->_source = data;
	first->_length = length;
	first->_type   = GRAM_FILENAME;
	first->_task   = trandom();
	first->_len    = strlen(storename);
	first->_num    = NLEN;
	first->_cnt    = count;
	
	memcpy ( (void*)first->_data, storename, first->_len );
	first->_data[first->_len] = '\0';

	return true;
}

bool
next ( Gram* gram, Gram* ngram ) {
	ngram->_source = gram->_source;
	ngram->_length = gram->_length;
	ngram->_task   = gram->_task;
	ngram->_cnt    = gram->_cnt;
	ngram->_num    = gram->_num;

	switch ( gram->_type ) {
		case GRAM_NIL      : { 
			ngram->_type = GRAM_NIL; 
			ngram->_len  = NLEN;
			ngram->_num  = NLEN;
			memcpy ( (void*)ngram->_data, (void*)((char*)ngram->_source+BUFFER_SIZE*(ngram->_num-1)), ngram->_len );    
			break; 
		}
		case GRAM_DATA     : {
			bool   transfinished = gram->_num==gram->_cnt;
			bool   lastgramp     = gram->_num==gram->_cnt-1;
			size_t startpos      = BUFFER_SIZE*(ngram->_num);

			ngram->_type = transfinished ? GRAM_NIL : GRAM_DATA; 
			ngram->_num  = transfinished ? NLEN     : gram->_num + 1;
			ngram->_len  = transfinished ? ( sizeof(NIL) ) : ( lastgramp ? gram->_length-startpos : BUFFER_SIZE );
			if ( ngram->_type==GRAM_DATA ) {
				memcpy ( (void*)ngram->_data, (void*)((char*)ngram->_source+startpos), ngram->_len ); 
			} else {
				memcpy ( (void*)ngram->_data, (void*)NIL, ngram->_len ); 
			}
			break; 
		} 
		case GRAM_LENGTH   : { 
			ngram->_type = GRAM_DATA;
			ngram->_num  = 1;
			ngram->_len  = ( ngram->_num>=ngram->_cnt ) ? ngram->_length-BUFFER_SIZE*gram->_num : BUFFER_SIZE;
			memcpy ( (void*)ngram->_data, ngram->_source, ngram->_len ); 			
			break; 
		} 
		case GRAM_FILENAME : { 
			ngram->_type = GRAM_DATA;
			ngram->_num  = 1;
			ngram->_len  = ( ngram->_num>=ngram->_cnt ) ? ngram->_length-BUFFER_SIZE*gram->_num : BUFFER_SIZE;
			memcpy ( (void*)ngram->_data, ngram->_source, ngram->_len ); 			
			break; 			
			ngram->_type = GRAM_LENGTH; 
			ngram->_num  = NLEN;
			ngram->_len  = sizeof(ngram->_length);
			memcpy ( (void*)ngram->_data, (void*)&ngram->_length, ngram->_len );
			break; 
		} 
		case GRAM_FEEDBACK : { 
			ngram->_type = GRAM_FEEDBACK; 
			ngram->_num  = NLEN;
			ngram->_len  = sizeof(FEEDBACK);
			memcpy ( (void*)ngram->_data, (void*)&FEEDBACK, ngram->_len );			 
			break; 
		}
		default            : { 
			ngram->_type = GRAM_NIL; 
			ngram->_num  = NLEN;
			ngram->_len  = NLEN;
			memcpy ( (void*)ngram->_data, (void*)&FEEDBACK, sizeof(FEEDBACK) );	   
			break; 
		}
	}

	return true;
}

int
valid ( const Gram* src, const Gram* valid ) {
	if ( src->_num != valid->_num ) return 1;
	if ( src->_cnt != valid->_cnt ) return 2;
	if ( src->_task != valid->_task ) return 3;
	if ( src->_length != valid->_length ) return 4;

	char srcsha1[SHA1SIZE], fdsha1[SHA1SIZE];
	sha1_hash ( (void*)src->_data, src->_len, srcsha1 );
	for ( int i = 0; i<valid->_len; i++ ) fdsha1[i] = (char)valid->_data[i];
	if ( memcmp ( (void*)srcsha1, (void*)valid->_data, valid->_len )!=0 ) return 5;
	
	return 0;
} 

bool
desc ( const Gram* gram, char* descstr ) {
	char gramtype[64];
	char format[] = "{ type:'%s', task:'%lu', len:'%lu', count:'%lu', num:'%lu', length:'%lu' }";

	switch ( gram->_type ) {
		case GRAM_NIL      : { sprintf(gramtype, "FINISH-MESSAGE"); break; }
		case GRAM_DATA     : { sprintf(gramtype, "DATA-CONTENT");   break; } 
		case GRAM_LENGTH   : { sprintf(gramtype, "DATA-LENGTH");    break; } 
		case GRAM_FILENAME : { sprintf(gramtype, "STORE-FILENAME"); break; } 
		case GRAM_FEEDBACK : { sprintf(gramtype, "FEEDBACK");       break; }
		default            : { sprintf(gramtype, "UNKNOW-TYPE");    break; }
	}
	sprintf ( descstr, format, gramtype, gram->_task, gram->_len, gram->_cnt, gram->_num==NLEN?0:gram->_num, gram->_length );

	return true;
}



