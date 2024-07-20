#ifndef _ROBOT_STORE_H_
#define _ROBOT_STORE_H_
#include <wiringPi.h>
#include <stdio.h>
#include "robot_common.h"
int  write_cell (MemoryCellType*);
int  read_cell (MemoryCellType*);
int  reverse_read_cell (MemoryCellType*);
int  open_memory_file_rd (unsigned short room_num);
int  reverse_open_memory_file_rd (unsigned short room_num);
int  open_memory_file_wr (unsigned short room_num);
void close_memory_file ();
#endif // _ROBOT_STORE_H_

