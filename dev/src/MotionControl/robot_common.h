
#ifndef _ROBOT_COMMON_H_
#define _ROBOT_COMMON_H_


#define MAXLINE 1024 

#define MV_FRWD            0x21
#define MV_BKWD            0x31
#define MV_LEFT            0x41
#define MV_RGHT            0x51
#define F_CAMERA_UP        0x52
#define F_CAMERA_DN        0x53
#define F_CAMERA_LFT       0x54
#define F_CAMERA_RGT       0x55
#define B_CAMERA_UP        0x56
#define B_CAMERA_DN        0x57
#define B_CAMERA_LFT       0x58
#define B_CAMERA_RGT       0x59
#define STOP               0x61
#define STOP_MOTION        0x62    // stop wheels
#define ACK                0x71
#define UP_SPEED       	   0x81
#define DN_SPEED	   0x91
#define SPEED	           0x92
#define END_RECORD         0x93
#define START_RECORD       0x94
#define ROOM_NO            0x95
#define REPLAY_MOTION      0x96
#define REVERSE_MOTION     0x97
#define RESTART	           0x99

typedef struct _MemoryCellType {
	unsigned int  actionDuration;   // mlisecond
	unsigned short actionType;
	unsigned short speed;

} MemoryCellType;

#endif // _ROBOT_COMMON_H
