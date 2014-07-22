#include "transfer.h"
#include "sha1.h"
#include "gram.h" 

void transferusage () {
    printf (
        "clitent $path-to-file-will-be-translated $path-storename-in-serverside $server-ip $server-port \n"
        "$path-to-file-will-be-translated : absolute path of file that will be transed \n"
        "$path-storename-in-serverside : if the server receive the content, where and by which name it will be stored \n"
        "$server-ip : internet address of the server listen on given port that will receiving the data \n"
        "$server-port : internet address port \n"
    );
}

int main ( int argc, char** argv ) {
    extern char GLOBAL_LOGMSG[MAX_LOG_MSG];     
    char filename[MAX_FILENAME]; 
    char storename[MAX_FILENAME]; 
    char receivehost[MAX_NAME_LEN];
    int  receiveport;

    if ( argc>1 ) {
        strcpy ( filename, argv[1] );
    } else {
        strcpy ( filename, CFILE_TEXT );
    }

    if ( argc>2 ) {
        strcpy ( storename, argv[2] );
    } else {
        char fname[MAX_FILENAME];
        char fpath[MAX_PATHNAME];
        char fext[MAX_NAME_LEN];
        char name[MAX_NAME_LEN];
        
        sprintf ( name, "%lu", trandom());
        pathname ( filename, fpath );
        extname ( filename, fext );
        strcpy ( fname, fpath );
        strcat ( fname, "/\0" );
        strcat ( fname, name );
        strcat ( fname, fext );

        strcpy ( storename, fname );
    }

    if ( argc>3 ) {
        strcpy ( receivehost, argv[3] );
    } else {
        strcpy ( receivehost, DEFAULT_HOST );
    }

    receiveport = argc>=4 ? atoi(argv[4]) : DEFAULT_PORT;
    FCONTENT fcontent = readfile (filename);

/*************************************************C中使用socket的固定部分：client－side：start******************************************************/
    int _sockfd;

    struct sockaddr_in _serv_addr;
    struct hostent*    _server;

    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if ( _sockfd<0 ) {
        transfererr ( "ERROR opening socket", TMSG_C );
    }

    _server = gethostbyname(receivehost);
    if ( _server==NULL ) {
        transfererr ( "ERROR, no such host", TMSG_C );
    }

    bzero ( (char*)&_serv_addr, sizeof(_serv_addr) );
    _serv_addr.sin_family = AF_INET;
    
    bcopy ( (char *)_server->h_addr_list[0], (char*)&_serv_addr.sin_addr.s_addr, _server->h_length ) ;
    _serv_addr.sin_port = htons(receiveport);

    if ( connect ( _sockfd, (struct sockaddr*)&_serv_addr, sizeof(_serv_addr)) < 0 ) {
        transfererr ( "ERROR connecting", TMSG_C );
    }
/*************************************************C中使用socket的固定部分：client－side：end******************************************************/

    Gram sending;   // 即将要发送的数据报文
    Gram receiving; // 接受反馈信息的数据报文
    int _return;

    char descstr[MAX_GRAM_DESC];    
    int rcnt = 0;
    int scnt = 0;

    bzero ( &sending, sizeof(sending) );
    first ( (void*)fcontent.pcontent, fcontent.length, storename, &sending ) ;

    bzero ( GLOBAL_LOGMSG, MAX_LOG_MSG ); 
    sprintf ( GLOBAL_LOGMSG, "transmission task %lu local : %s ; remote : %s ; length: %lu ", sending._task, filename, storename, sending._length ); transfererr ( GLOBAL_LOGMSG, TMSG_D );    

    while ( true ) {
        _return = write ( _sockfd, &sending, sizeof(sending) );
  
        bzero ( GLOBAL_LOGMSG, MAX_LOG_MSG ); desc ( &sending, descstr ); 
        sprintf ( GLOBAL_LOGMSG, "client side sent %d gram: %s", ++scnt, descstr ); transfererr ( GLOBAL_LOGMSG, TMSG_D );

        if ( _return<0 ) {
            transfererr ( "ERROR writing to socket", TMSG_C );
        }
        
        bzero ( &receiving, sizeof(receiving) );
        _return = read ( _sockfd, (void*)&receiving, GRAM_MIN_SIZE+SHA1SIZE ) ;
     
        bzero ( GLOBAL_LOGMSG, MAX_LOG_MSG ); desc ( &receiving, descstr ); 
        sprintf ( GLOBAL_LOGMSG, "client side received %d gram: %s", ++rcnt, descstr ); transfererr ( GLOBAL_LOGMSG, TMSG_D );
        
        if ( _return<0 ) {
            transfererr ( "ERROR reading from socket", TMSG_C );
        } else {
            if ( sending._type==GRAM_NIL ) {
                break; 
            }

            int err = valid ( &sending, &receiving );
            if ( !err ) {
                next ( &sending, &sending );
            } else {
                bzero ( GLOBAL_LOGMSG, MAX_LOG_MSG ); 
                sprintf ( GLOBAL_LOGMSG, "error ocurrence when transmission for error %d", err ); transfererr ( GLOBAL_LOGMSG, TMSG_D );
            }
        }
    }

    bzero ( GLOBAL_LOGMSG, MAX_LOG_MSG ); 
    sprintf ( GLOBAL_LOGMSG, "transmission task %lu finished. ", sending._task ); transfererr ( GLOBAL_LOGMSG, TMSG_D );
   
    return 0;
}




