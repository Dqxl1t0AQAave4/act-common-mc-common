#pragma once

#include <act-common/iobuf.h>


typedef iobuf<byte, 3u, byte> buf_t;


const char * iobuf_test_messages [32];
      bool   iobuf_test_results  [32];


inline void iobuf_read_test0(byte &i)
{
    buf_t src;
    byte dst[3];
    byte r;
    
    // ------ 1 2 3 4
    
         dst[0] = 0;      dst[1] = 1;      dst[2] = 2;
    src.data[0] = 3; src.data[1] = 4; src.data[2] = 5;
    src.position = 0;
    src.length = 0;
    
    // should not do anything
    r = iobuf_read<sp_read_any, lp_adopt_lock>(dst, src, (byte) 2);
    
    iobuf_test_results[i++] = (r == 0);
    iobuf_test_results[i++] = (dst[0] == 0) && (dst[1] == 1) && (dst[2] == 2);
    iobuf_test_results[i++] = (src.position == 0);
    iobuf_test_results[i++] = (src.length == 0);
    
    // ------ 5 6 7 8
    
         dst[0] = 0;      dst[1] = 1;      dst[2] = 2;
    src.data[0] = 3; src.data[1] = 4; src.data[2] = 5;
    src.position = 0;
    src.length = 3;
    
    // should read 2 bytes
    r = iobuf_read<sp_read_any, lp_adopt_lock>(dst, src, (byte) 2);
    
    iobuf_test_results[i++] = (r == 2);
    iobuf_test_results[i++] = (dst[0] == 3) && (dst[1] == 4) && (dst[2] == 2);
    iobuf_test_results[i++] = (src.position == 2);
    iobuf_test_results[i++] = (src.length == 1);
    
    // ------ 9 10 11 12
    
         dst[0] = 0;      dst[1] = 1;      dst[2] = 2;
    src.data[0] = 3; src.data[1] = 4; src.data[2] = 5;
    src.position = 0;
    src.length = 1;
    
    // should read 1 byte
    r = iobuf_read<sp_read_any, lp_adopt_lock>(dst, src, (byte) 2);
    
    iobuf_test_results[i++] = (r == 1);
    iobuf_test_results[i++] = (dst[0] == 3) && (dst[1] == 1) && (dst[2] == 2);
    iobuf_test_results[i++] = (src.position == 1);
    iobuf_test_results[i++] = (src.length == 0);
    
    // ------ 13 14 15 16
    
         dst[0] = 0;      dst[1] = 1;      dst[2] = 2;
    src.data[0] = 3; src.data[1] = 4; src.data[2] = 5;
    src.position = 2;
    src.length = 3;
    
    // should read 2 bytes
    r = iobuf_read<sp_read_any, lp_adopt_lock>(dst, src, (byte) 2);
    
    iobuf_test_results[i++] = (r == 2);
    iobuf_test_results[i++] = (dst[0] == 5) && (dst[1] == 3) && (dst[2] == 2);
    iobuf_test_results[i++] = (src.position == 1);
    iobuf_test_results[i++] = (src.length == 1);
    
    // ------ 17 18 19 20
    
         dst[0] = 0;      dst[1] = 1;      dst[2] = 2;
    src.data[0] = 3; src.data[1] = 4; src.data[2] = 5;
    src.position = 2;
    src.length = 1;
    
    // should read 1 byte
    r = iobuf_read<sp_read_any, lp_adopt_lock>(dst, src, (byte) 2);
    
    iobuf_test_results[i++] = (r == 1);
    iobuf_test_results[i++] = (dst[0] == 5) && (dst[1] == 1) && (dst[2] == 2);
    iobuf_test_results[i++] = (src.position == 0);
    iobuf_test_results[i++] = (src.length == 0);
    
    // ------
}


inline void iobuf_test()
{
    byte i = 1;
    iobuf_read_test0(i);
}