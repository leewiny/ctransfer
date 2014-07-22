#include "transfer.h"
#include "sha1.h"
#include "gram.h"

void mkfilecheck ( int argc, char** argv ) {
    char storepath[] = "tmp/2014/06/27/";
    char storename[MAX_FILENAME];
    unsigned long fname = trandom();
    char filename[MAX_NAME_LEN];
    for ( int i=0; i<=10; i++ ) {
        fname++;
        hexstr ( (void*)&fname, sizeof(fname), filename );
        sprintf ( storename, "%s%s", storepath, filename );
        makefile ( storename );
        printf ( "make file %s\n", storename );
    }
}

int main ( int argc, char** argv ) { 
    mkfilecheck ( argc, argv );
    return 0; 
}
