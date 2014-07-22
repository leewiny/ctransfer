#include "transfer.h"

unsigned long timestamp () {
	long timezone = 0;
	struct tm stasc = {0,0,0,1,0,1900,0,0,0,timezone,0};
	time_t current = time(NULL);
	time_t oldest  = mktime(&stasc);
	return (unsigned long)current;
	return (unsigned long)difftime(current, oldest);
}

unsigned long trandom () {
	clock_t tick = clock();
	tick += tick%10+(tick%2?100:1000);
	srand ( tick );   
	return (unsigned long)rand();
}

void transfererr ( const char* msg, const TRANSMSG code ) { transfermsg ( msg, code ); }
void transfermsg ( const char* msg, const TRANSMSG level ) {
    if ( level>=TMSG_LEVEL ) {
        struct tm* ptime;
        time_t logtime;
        char timestr[128];

        logtime = time ( NULL );
        ptime   = localtime ( &logtime );
        strftime ( timestr, 128, "trans-%Y%m%d.log", ptime);

        char logfilename[MAX_FILENAME];
        bzero ( logfilename, MAX_FILENAME );
        strcat ( logfilename, LOG_FILE );
        strcat ( logfilename, timestr );
        makefile ( logfilename ); 
        FILE* translog = fopen ( logfilename, "a" ); 
        if ( translog==NULL ) {
            printf ( "can not open log file %s, program will exist ......\n", logfilename );
            exit(3);
        } else {
            bzero ( timestr, 128 );
            strftime ( timestr, 128, "%Y-%m-%d~%H:%M:%S\t", ptime);

            fwrite ( timestr, 1, strlen(timestr), translog );
            fwrite ( msg, 1, strlen(msg), translog );
            fwrite ( "\n", 1, 1, translog );
            fclose ( translog );

#ifdef TMSG_STD
            printf ( "%s%s\n", timestr,msg );
#endif                        
        }
    }

    if ( level>=TMSG_C ) {
        exit ( TMSG_C );
    }
}

bool makefile ( const char* filename ) {
    if ( -1==access(filename,F_OK) ) {
        char storename[MAX_FILENAME]; 
        char storepath[MAX_FILENAME];
        char ch;

        bzero ( storename, MAX_FILENAME );
        bzero ( storepath, MAX_FILENAME );
        absname ( filename, storename );

        for ( int i=0; i<strlen(storename); i++ ) {
            ch = storename[i];
            if ( ch=='/' ) {
                if ( -1==access(storepath,F_OK) ) {
                    mkdir ( storepath, 0777 );
                    chmod ( storepath, 07777 );
                }
            }
            storepath[i] = ch;
        }

        if ( -1==access(storepath,F_OK) ) {
            FILE* fp = fopen ( storepath, "wb" ) ;
            chmod ( storepath, 07777 );
            fclose ( fp );
        }
    }

    return true;
}

FCONTENT readfile ( const char* filename ) {
    FILE* creader = fopen ( filename, "rb" ) ;  
    FCONTENT fcontent;
    fcontent.length   = NLEN;
    fcontent.pcontent = NULL;
    
    size_t flength = 0;
    if ( creader!=NULL ) {
        fseek ( creader, 0, SEEK_END );
        flength = ftell ( creader ) ;
        fseek ( creader, 0 , SEEK_SET );

        void* buffer = malloc(flength);
        if ( buffer!=NULL ) {
            fread ( buffer, 1, flength, creader);
            fcontent.pcontent = buffer;
            fcontent.length   = flength;
        }

        fclose(creader);
    }

    return fcontent;
}

size_t filelength ( const char* filename ) {
    FILE* creader = fopen ( filename, "rb" ) ;  
    size_t flength = NLEN;
    if ( creader!=NULL ) {
        fseek ( creader, 0, SEEK_END );
        flength = ftell ( creader ) ;
        fclose(creader);
    }

    return flength;    
}

size_t store ( const void* content, size_t length, const char* filename ) {
    bzero ( GLOBAL_LOGMSG, MAX_LOG_MSG ); 
    sprintf ( GLOBAL_LOGMSG, "write %lu bytes to file %s ", length, filename ); transfererr ( GLOBAL_LOGMSG, TMSG_N );
    makefile ( filename );
    FILE* writer = fopen ( filename, "wb" ) ;  
    if ( writer!=NULL ) {
        size_t written = fwrite ( content, 1, length, writer );
        fclose ( writer );
        return written;
    } else {
        return 0;
    }
}

int basename ( const char* filename, char* basename ) {
    char* lastsep = strrchr ( filename, '/' );
    if ( lastsep ) {
        strcpy ( basename, lastsep+1 );
        return strlen (basename);
    } else {
        return strlen(filename);
    }
}

int pathname ( const char* filename, char* pathname ) {
    absname ( filename, pathname );
    int pos = strlen(pathname);
    for ( ; pathname[pos]!='/'; pos-- );
    pathname[pos] = '\0';
    return strlen(pathname);
}

int absname ( const char* filename, char* absname ) {
    size_t flen = strlen(filename);
    char staticname[flen]; 
    char* myfname = staticname; 
    strcpy ( myfname, filename );
    while ( *myfname==' ' ) myfname++;
    if ( *myfname=='/' ) {
        strcpy ( absname, myfname );
        return strlen(absname);
    } else {
        char cwd[MAX_FILENAME];
        getcwd ( cwd, MAX_FILENAME );
        strcpy ( absname, cwd );
        strcat ( absname, "/\0" );
        strcat ( absname, filename );
        return strlen(absname);
    }
}

int extname ( const char* filename, char* extname ) {
    char* lastdot = strrchr ( filename, '.' );
    if ( lastdot ) {
        strcpy ( extname, lastdot );
        int length = strlen(extname);
        for ( int i=0; i<length; i++ ) extname[i] =  isupper(extname[i]) ? tolower(extname[i]) : extname[i];
        return length;
    } else {
        return 0;
    }
}

size_t hexstr ( void* value, size_t length, char* store ) {
    if ( length>0 ) {
        size_t j = length * 2;
        size_t rtn = j;

        memset ( (void*)store, '0', j );
        store[j--] = '\0';
        for ( size_t i=0; i<length; i++ ) {
            unsigned char ch = (unsigned char)(((char*)value)[i]);
            store[j--] = ch%HEX_BASE>=10 ? 'A'+ch%HEX_BASE-10 : ch%HEX_BASE+'0';
            ch /= HEX_BASE;
            store[j--] = ch%HEX_BASE>=10 ? 'A'+ch%HEX_BASE-10 : ch%HEX_BASE+'0';
        }

        return rtn;
    } else {
        return 0;
    }
}

void quicksort ( int* array, int count ) { 
    if ( count<=1 ) return;

    int li  = 0;
    int ri  = count - 1;
    int mid = *array; 

    while ( li!=ri ) {
        while ( array[li]>mid  ) { 
            if ( li==ri ) break;
            if ( li<count-1 && li<ri ) li++; 
        }

        while ( array[ri]<=mid ) { 
            if ( li==ri ) break; 
            if ( ri>0 && li<ri ) ri--;
        }

        if ( li==ri ) {
            break;
        } else {
            array[li] = array[li]^array[ri];
            array[ri] = array[li]^array[ri];
            array[li] = array[li]^array[ri];
        }
    }

    if ( 0<=li && li<=(count-1) ) { 
        if ( li==0 ) { 
            quicksort ( array+1, count-1 );
        } else if ( li==count-1 ) {
            quicksort ( array, count-1 );   
        } else {
            quicksort ( array, li );
            quicksort ( array+li, count-li );
        }
    }

    return;
}

void bubblesort ( int* array, int count ) {
    if ( count<=0 ) return;
    for ( int i=0; i<=count-2; i++ ) {
        for ( int j=i+1; j<=count-1; j++ ) {
            if ( array[i]<array[j] ) {
                array[i] = array[i]^array[j];
                array[j] = array[i]^array[j];
                array[i] = array[i]^array[j];
            }       
        }
    }

    return;
}

