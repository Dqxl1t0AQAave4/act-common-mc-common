#pragma once

#include <act-common/iobuf.h>


typedef iobuf<byte, 3u, byte> buf_t;


bool iobuf_test_results[128];


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
    r = iobuf_read<sp_process_any, lp_adopt_lock>(dst, src, (byte) 2);
    
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
    r = iobuf_read<sp_process_any, lp_adopt_lock>(dst, src, (byte) 2);
    
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
    r = iobuf_read<sp_process_any, lp_adopt_lock>(dst, src, (byte) 2);
    
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
    r = iobuf_read<sp_process_any, lp_adopt_lock>(dst, src, (byte) 2);
    
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
    r = iobuf_read<sp_process_any, lp_adopt_lock>(dst, src, (byte) 2);
    
    iobuf_test_results[i++] = (r == 1);
    iobuf_test_results[i++] = (dst[0] == 5) && (dst[1] == 1) && (dst[2] == 2);
    iobuf_test_results[i++] = (src.position == 0);
    iobuf_test_results[i++] = (src.length == 0);
    
    // ------
}


inline void iobuf_read_test1(byte &i)
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
    r = iobuf_read<sp_process_full, lp_adopt_lock>(dst, src, (byte) 2);
    
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
    r = iobuf_read<sp_process_full, lp_adopt_lock>(dst, src, (byte) 2);
    
    iobuf_test_results[i++] = (r == 2);
    iobuf_test_results[i++] = (dst[0] == 3) && (dst[1] == 4) && (dst[2] == 2);
    iobuf_test_results[i++] = (src.position == 2);
    iobuf_test_results[i++] = (src.length == 1);
    
    // ------ 9 10 11 12
    
         dst[0] = 0;      dst[1] = 1;      dst[2] = 2;
    src.data[0] = 3; src.data[1] = 4; src.data[2] = 5;
    src.position = 0;
    src.length = 1;
    
    // should read 0 bytes => full restriction
    r = iobuf_read<sp_process_full, lp_adopt_lock>(dst, src, (byte) 2);
    
    iobuf_test_results[i++] = (r == 0);
    iobuf_test_results[i++] = (dst[0] == 0) && (dst[1] == 1) && (dst[2] == 2);
    iobuf_test_results[i++] = (src.position == 0);
    iobuf_test_results[i++] = (src.length == 1);
    
    // ------ 13 14 15 16
    
         dst[0] = 0;      dst[1] = 1;      dst[2] = 2;
    src.data[0] = 3; src.data[1] = 4; src.data[2] = 5;
    src.position = 2;
    src.length = 3;
    
    // should read 2 bytes
    r = iobuf_read<sp_process_full, lp_adopt_lock>(dst, src, (byte) 2);
    
    iobuf_test_results[i++] = (r == 2);
    iobuf_test_results[i++] = (dst[0] == 5) && (dst[1] == 3) && (dst[2] == 2);
    iobuf_test_results[i++] = (src.position == 1);
    iobuf_test_results[i++] = (src.length == 1);
    
    // ------ 17 18 19 20
    
         dst[0] = 0;      dst[1] = 1;      dst[2] = 2;
    src.data[0] = 3; src.data[1] = 4; src.data[2] = 5;
    src.position = 2;
    src.length = 1;
    
    // should read 0 bytes => full restriction
    r = iobuf_read<sp_process_full, lp_adopt_lock>(dst, src, (byte) 2);
    
    iobuf_test_results[i++] = (r == 0);
    iobuf_test_results[i++] = (dst[0] == 0) && (dst[1] == 1) && (dst[2] == 2);
    iobuf_test_results[i++] = (src.position == 2);
    iobuf_test_results[i++] = (src.length == 1);
    
    // ------ 21 22 23 24
    
         dst[0] = 0;      dst[1] = 1;      dst[2] = 2;
    src.data[0] = 3; src.data[1] = 4; src.data[2] = 5;
    src.position = 2;
    src.length = 1;
    
    // should read 1 byte
    r = iobuf_read<sp_process_full, lp_adopt_lock>(dst, src, (byte) 1);
    
    iobuf_test_results[i++] = (r == 1);
    iobuf_test_results[i++] = (dst[0] == 5) && (dst[1] == 1) && (dst[2] == 2);
    iobuf_test_results[i++] = (src.position == 0);
    iobuf_test_results[i++] = (src.length == 0);
    
    // ------
}



inline void iobuf_read_test2(byte &i)
{
    buf_t src;
    byte dst;
    byte r;
    
    // ------ 1 2 3 4
    
    dst = 0;
    src.data[0] = 3; src.data[1] = 4; src.data[2] = 5;
    src.position = 0;
    src.length = 0;
    
    // should not do anything
    r = iobuf_read<lp_adopt_lock>(dst, src);
    
    iobuf_test_results[i++] = (r == 0);
    iobuf_test_results[i++] = (dst == 0);
    iobuf_test_results[i++] = (src.position == 0);
    iobuf_test_results[i++] = (src.length == 0);
    
    // ------ 5 6 7 8
    
    dst = 0;
    src.data[0] = 3; src.data[1] = 4; src.data[2] = 5;
    src.position = 0;
    src.length = 3;
    
    // should read 1 byte
    r = iobuf_read<lp_adopt_lock>(dst, src);
    
    iobuf_test_results[i++] = (r == 1);
    iobuf_test_results[i++] = (dst == 3);
    iobuf_test_results[i++] = (src.position == 1);
    iobuf_test_results[i++] = (src.length == 2);
    
    // ------ 9 10 11 12
    
    dst = 0;
    src.data[0] = 3; src.data[1] = 4; src.data[2] = 5;
    src.position = 2;
    src.length = 1;
    
    // should read 1 byte
    r = iobuf_read<lp_adopt_lock>(dst, src);
    
    iobuf_test_results[i++] = (r == 1);
    iobuf_test_results[i++] = (dst == 5);
    iobuf_test_results[i++] = (src.position == 0);
    iobuf_test_results[i++] = (src.length == 0);
    
    // ------
}


inline void iobuf_write_test0(byte &i)
{
    buf_t dst;
    byte src[3];
    byte r;
    
    // ------ 1 2 3 4
    
         src[0] = 0;      src[1] = 1;      src[2] = 2;
    dst.data[0] = 3; dst.data[1] = 4; dst.data[2] = 5;
    dst.position = 0;
    dst.length = 3;
    
    // should not do anything
    r = iobuf_write<sp_process_any, lp_adopt_lock>(dst, src, (byte) 2);
    
    iobuf_test_results[i++] = (r == 0);
    iobuf_test_results[i++] = (dst[0] == 3) && (dst[1] == 4) && (dst[2] == 5);
    iobuf_test_results[i++] = (dst.position == 0);
    iobuf_test_results[i++] = (dst.length == 3);
    
    // ------ 5 6 7 8
    
         src[0] = 0;      src[1] = 1;      src[2] = 2;
    dst.data[0] = 3; dst.data[1] = 4; dst.data[2] = 5;
    dst.position = 0;
    dst.length = 0;
    
    // should write 2 bytes
    r = iobuf_write<sp_process_any, lp_adopt_lock>(dst, src, (byte) 2);
    
    iobuf_test_results[i++] = (r == 2);
    iobuf_test_results[i++] = (dst[0] == 0) && (dst[1] == 1) && (dst[2] == 5);
    iobuf_test_results[i++] = (dst.position == 0);
    iobuf_test_results[i++] = (dst.length == 2);
    
    // ------ 9 10 11 12
    
         src[0] = 0;      src[1] = 1;      src[2] = 2;
    dst.data[0] = 3; dst.data[1] = 4; dst.data[2] = 5;
    dst.position = 0;
    dst.length = 2;
    
    // should write 1 byte
    r = iobuf_write<sp_process_any, lp_adopt_lock>(dst, src, (byte) 2);
    
    iobuf_test_results[i++] = (r == 1);
    iobuf_test_results[i++] = (dst[0] == 3) && (dst[1] == 4) && (dst[2] == 0);
    iobuf_test_results[i++] = (dst.position == 0);
    iobuf_test_results[i++] = (dst.length == 3);
    
    // ------ 13 14 15 16
    
         src[0] = 0;      src[1] = 1;      src[2] = 2;
    dst.data[0] = 3; dst.data[1] = 4; dst.data[2] = 5;
    dst.position = 2;
    dst.length = 0;
    
    // should write 2 bytes
    r = iobuf_write<sp_process_any, lp_adopt_lock>(dst, src, (byte) 2);
    
    iobuf_test_results[i++] = (r == 2);
    iobuf_test_results[i++] = (dst[0] == 1) && (dst[1] == 4) && (dst[2] == 0);
    iobuf_test_results[i++] = (dst.position == 2);
    iobuf_test_results[i++] = (dst.length == 2);
    
    // ------ 17 18 19 20
    
         src[0] = 0;      src[1] = 1;      src[2] = 2;
    dst.data[0] = 3; dst.data[1] = 4; dst.data[2] = 5;
    dst.position = 2;
    dst.length = 2;
    
    // should write 1 byte
    r = iobuf_write<sp_process_any, lp_adopt_lock>(dst, src, (byte) 2);
    
    iobuf_test_results[i++] = (r == 1);
    iobuf_test_results[i++] = (dst[0] == 3) && (dst[1] == 0) && (dst[2] == 5);
    iobuf_test_results[i++] = (dst.position == 2);
    iobuf_test_results[i++] = (dst.length == 3);
    
    // ------
}


inline void iobuf_write_test1(byte &i)
{
    buf_t dst;
    byte src[3];
    byte r;
    
    // ------ 1 2 3 4
    
         src[0] = 0;      src[1] = 1;      src[2] = 2;
    dst.data[0] = 3; dst.data[1] = 4; dst.data[2] = 5;
    dst.position = 0;
    dst.length = 3;
    
    // should not do anything
    r = iobuf_write<sp_process_full, lp_adopt_lock>(dst, src, (byte) 2);
    
    iobuf_test_results[i++] = (r == 0);
    iobuf_test_results[i++] = (dst[0] == 3) && (dst[1] == 4) && (dst[2] == 5);
    iobuf_test_results[i++] = (dst.position == 0);
    iobuf_test_results[i++] = (dst.length == 3);
    
    // ------ 5 6 7 8
    
         src[0] = 0;      src[1] = 1;      src[2] = 2;
    dst.data[0] = 3; dst.data[1] = 4; dst.data[2] = 5;
    dst.position = 0;
    dst.length = 0;
    
    // should write 2 bytes
    r = iobuf_write<sp_process_full, lp_adopt_lock>(dst, src, (byte) 2);
    
    iobuf_test_results[i++] = (r == 2);
    iobuf_test_results[i++] = (dst[0] == 0) && (dst[1] == 1) && (dst[2] == 5);
    iobuf_test_results[i++] = (dst.position == 0);
    iobuf_test_results[i++] = (dst.length == 2);
    
    // ------ 9 10 11 12
    
         src[0] = 0;      src[1] = 1;      src[2] = 2;
    dst.data[0] = 3; dst.data[1] = 4; dst.data[2] = 5;
    dst.position = 0;
    dst.length = 2;
    
    // should write 0 bytes => full restriction
    r = iobuf_write<sp_process_full, lp_adopt_lock>(dst, src, (byte) 2);
    
    iobuf_test_results[i++] = (r == 0);
    iobuf_test_results[i++] = (dst[0] == 3) && (dst[1] == 4) && (dst[2] == 5);
    iobuf_test_results[i++] = (dst.position == 0);
    iobuf_test_results[i++] = (dst.length == 2);
    
    // ------ 13 14 15 16
    
         src[0] = 0;      src[1] = 1;      src[2] = 2;
    dst.data[0] = 3; dst.data[1] = 4; dst.data[2] = 5;
    dst.position = 2;
    dst.length = 0;
    
    // should write 2 bytes
    r = iobuf_write<sp_process_full, lp_adopt_lock>(dst, src, (byte) 2);
    
    iobuf_test_results[i++] = (r == 2);
    iobuf_test_results[i++] = (dst[0] == 1) && (dst[1] == 4) && (dst[2] == 0);
    iobuf_test_results[i++] = (dst.position == 2);
    iobuf_test_results[i++] = (dst.length == 2);
    
    // ------ 17 18 19 20
    
         src[0] = 0;      src[1] = 1;      src[2] = 2;
    dst.data[0] = 3; dst.data[1] = 4; dst.data[2] = 5;
    dst.position = 2;
    dst.length = 2;
    
    // should write 0 bytes => full restriction
    r = iobuf_write<sp_process_full, lp_adopt_lock>(dst, src, (byte) 2);
    
    iobuf_test_results[i++] = (r == 0);
    iobuf_test_results[i++] = (dst[0] == 3) && (dst[1] == 4) && (dst[2] == 5);
    iobuf_test_results[i++] = (dst.position == 2);
    iobuf_test_results[i++] = (dst.length == 2);
    
    // ------ 21 22 23 24
    
         src[0] = 0;      src[1] = 1;      src[2] = 2;
    dst.data[0] = 3; dst.data[1] = 4; dst.data[2] = 5;
    dst.position = 2;
    dst.length = 2;
    
    // should write 1 byte
    r = iobuf_write<sp_process_full, lp_adopt_lock>(dst, src, (byte) 1);
    
    iobuf_test_results[i++] = (r == 1);
    iobuf_test_results[i++] = (dst[0] == 3) && (dst[1] == 0) && (dst[2] == 5);
    iobuf_test_results[i++] = (dst.position == 2);
    iobuf_test_results[i++] = (dst.length == 3);
    
    // ------
}


inline void iobuf_write_test2(byte &i)
{
    buf_t dst;
    byte src;
    byte r;
    
    // ------ 1 2 3 4
    
    src = 6;
    dst.data[0] = 3; dst.data[1] = 4; dst.data[2] = 5;
    dst.position = 0;
    dst.length = 3;
    
    // should not do anything
    r = iobuf_write<lp_adopt_lock>(dst, src);
    
    iobuf_test_results[i++] = (r == 0);
    iobuf_test_results[i++] = (dst[0] == 3) && (dst[1] == 4) && (dst[2] == 5);
    iobuf_test_results[i++] = (dst.position == 0);
    iobuf_test_results[i++] = (dst.length == 3);
    
    // ------ 5 6 7 8
    
    src = 6;
    dst.data[0] = 3; dst.data[1] = 4; dst.data[2] = 5;
    dst.position = 0;
    dst.length = 2;
    
    // should write 1 byte
    r = iobuf_write<lp_adopt_lock>(dst, src);
    
    iobuf_test_results[i++] = (r == 1);
    iobuf_test_results[i++] = (dst[0] == 3) && (dst[1] == 4) && (dst[2] == 6);
    iobuf_test_results[i++] = (dst.position == 0);
    iobuf_test_results[i++] = (dst.length == 3);
    
    // ------ 9 10 11 12
    
    src = 6;
    dst.data[0] = 3; dst.data[1] = 4; dst.data[2] = 5;
    dst.position = 2;
    dst.length = 2;
    
    // should write 1 byte
    r = iobuf_write<lp_adopt_lock>(dst, src);
    
    iobuf_test_results[i++] = (r == 1);
    iobuf_test_results[i++] = (dst[0] == 3) && (dst[1] == 6) && (dst[2] == 5);
    iobuf_test_results[i++] = (dst.position == 2);
    iobuf_test_results[i++] = (dst.length == 3);
    
    // ------
}


inline void iobuf_test()
{
    byte i = 1;
    
    iobuf_read_test0(i);
    iobuf_read_test1(i);
    iobuf_read_test2(i);
    
    iobuf_write_test0(i);
    iobuf_write_test1(i);
    iobuf_write_test2(i);
}