#include "transfer.h"
#include "sha1.h"
#include "gram.h"

void receive ( const int sckt ) {
    extern char GLOBAL_LOGMSG[MAX_LOG_MSG]; 
    char filename[MAX_FILENAME];
    char*  _data = NULL;

    int    _return   = 0;
    size_t _length   = 0;
    size_t _position = 0;

    Gram received; // 接收到的数据包
    Gram sending;  // 反馈的数据包

    bool finished = false;
    char descstr[MAX_GRAM_DESC];  
    int rcnt = 0;
    int scnt = 0;

    bzero ( filename, MAX_FILENAME );
    do {
        bzero ( &received, sizeof(Gram) );
        _return = read ( sckt, &received, sizeof(Gram) );

        bzero ( GLOBAL_LOGMSG, MAX_LOG_MSG ); desc ( &received, descstr ); 
        sprintf ( GLOBAL_LOGMSG, "server side received %d gram of task %lu : %s", ++rcnt, received._task, descstr ); transfererr ( GLOBAL_LOGMSG, TMSG_D );

        if ( _return < 0 ) {
            transfererr ( "ERROR reading from socket", TMSG_E ) ;
            return;
        } else {
/*************************************************C中使用socket的固定部分：server－side：start******************************************************/            
            if ( received._type==GRAM_DATA ) {
                memcpy ( (void*)((char*)_data+_position), received._data, received._len );
                _position += received._len;                
            } else if ( received._type==GRAM_FILENAME ) {
                memcpy ( (void*)filename, received._data, received._len );
                filename[received._len] = '\0';
                _length = received._length;
                _data = (char*)malloc (_length);
            } else if ( received._type==GRAM_LENGTH ) {
                memcpy ( (void*)_length, received._data, received._len );
                _data = malloc (_length);
            } else if ( received._type==GRAM_NIL ) {
                finished = true;
            }

            bzero ( &sending, sizeof(Gram) );
            if ( !feedback ( &received, &sending ) ) { 
                transfererr ( "feedback error", TMSG_E ); 
                return;
            }
            
            _return = write ( sckt, &sending, GRAM_MIN_SIZE+SHA1SIZE ) ;
/*************************************************C中使用socket的固定部分：server－side：end******************************************************/
            bzero ( GLOBAL_LOGMSG, MAX_LOG_MSG ); desc ( &sending, descstr );  
            sprintf ( GLOBAL_LOGMSG, "server side sent %d gram of task %lu : %s ", ++scnt, sending._task, descstr ); transfererr ( GLOBAL_LOGMSG, TMSG_D );            

            if ( _return < 0 ) {
                transfererr ( "ERROR writing to socket", TMSG_E );
                return;
            }
        }
    } while ( !finished );

    if ( _position!=_length ) {
        bzero ( GLOBAL_LOGMSG, MAX_LOG_MSG ); 
        sprintf ( GLOBAL_LOGMSG, "received length (%lu) NOT EQUAL report length %lu of task %lu", _length, _position, received._task ); transfererr ( GLOBAL_LOGMSG, TMSG_E );
    } else {
        bzero ( GLOBAL_LOGMSG, MAX_LOG_MSG ); 
        sprintf ( GLOBAL_LOGMSG, "task %lu transmission finished", received._task ); transfererr ( GLOBAL_LOGMSG, TMSG_D );     
        store ( (void*)_data, _length, filename );
    }
    close ( sckt );   	
}

int main ( int argc, char** argv ) {
/*************************************************C中使用socket的固定部分：server－side：start******************************************************/
    pid_t rtnpid = fork ();
    if ( rtnpid<0 ) { // if error
        printf ( "can not fork child process" );
        return 1;
    } else if ( rtnpid>0 ) { // return in parent process
        return 0;
    } else { // return in child process
        int _sockfd;

        struct sockaddr_in _serv_addr;
        struct sockaddr_in _cli_addr;

        _sockfd = socket ( AF_INET, SOCK_STREAM, 0 );
        if ( _sockfd<0 ) transfererr ( "ERROR opening socket", TMSG_F );

        bzero ( (char*)&_serv_addr, sizeof(_serv_addr) );
        _serv_addr.sin_family      = AF_INET;
        _serv_addr.sin_addr.s_addr = INADDR_ANY;
        _serv_addr.sin_port        = htons(DEFAULT_PORT);
         
        int rtn = bind ( _sockfd, (struct sockaddr*)&_serv_addr, sizeof(_serv_addr) );
        if ( rtn<0 ) transfererr ( "ERROR on binding", TMSG_F );

        listen ( _sockfd, 5 );
        socklen_t clilen = sizeof(_cli_addr);
        while ( true ) {
            int csockfd = accept ( _sockfd, (struct sockaddr*)&_cli_addr, &clilen );
            if ( csockfd<0 ) { 
                transfererr ( "ERROR on accept", TMSG_E ) ; 
                continue; 
            } else { 
                receive ( csockfd );
            }
        }

        close ( _sockfd );
    }
/*************************************************C中使用socket的固定部分：server－side：end******************************************************/    
}




