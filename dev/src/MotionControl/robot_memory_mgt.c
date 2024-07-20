//unsigned int millis (void)
#include "robot_common.h"
#include "robot_memory_mgt.h"
FILE *a_file = NULL;
int current_pos = 0;
//size_t fread(void *ptr, size_t size_of_elements, size_t number_of_elements, FILE *a_file);

int write_cell (MemoryCellType *cell){
	int size;
	if (a_file != NULL){
	    size = fwrite(cell, sizeof(MemoryCellType), 1, a_file);
	    fflush (a_file);
	    return (size);
	}
	return (0);
}

int reverse_open_memory_file_rd_fromendfile (unsigned short room_num){
    char file_name [256];
    sprintf (file_name,"/usr/local/etc/smartrobot/room%02d",room_num);
    printf ("file name %s \n",file_name);
    if ((a_file = fopen (file_name, "r")) != NULL){
	fseek(a_file, 0, SEEK_END);
	current_pos = ftell(a_file);
        printf ("reverse_open_memory_file_rd: reverse pos %d \n",current_pos);
	if (current_pos == 0){ /* file is empty */ ;
		  return (0);
	}
        return 1;
    }
    return 0;
}


int reverse_open_memory_file_rd (unsigned short room_num){
    char file_name [256];
    sprintf (file_name,"/usr/local/etc/smartrobot/room%02d",room_num);
    printf ("file name %s \n",file_name);
    if ((a_file = fopen (file_name, "r")) != NULL){
        return 1;
    }
    return 0;
}


int reverse_read_cell (MemoryCellType *cell){

        int cellsize = sizeof (MemoryCellType);
        printf ("reverse_read_cell: reverse pos %d \n",current_pos);
	if (current_pos <= 0)
		return 0;
        current_pos -= cellsize;
	if (current_pos >= 0) {
    		fseek(a_file, current_pos, SEEK_SET);
    		size_t cnt = fread(cell, cellsize, 1, a_file);
   		 // process buf
		printf ("reverse_read_cell: reverse_pos %d cnt %d\n",current_pos,cnt);
		return (1);
	}
   return (0);	
}



int read_cell (MemoryCellType *cell){
	int size;
        int cellsize = sizeof (MemoryCellType);
	if (a_file != NULL){
    	        if (fseek(a_file, current_pos, SEEK_SET) >= 0){
			if (!feof (a_file)){
				size = fread(cell, cellsize, 1, a_file);
				current_pos += cellsize;
				return size;
			}
			else {
				printf ("end of file");
				return 0;
			}
	        	printf ("read_cell: size %d  elements read %d\n",sizeof(MemoryCellType),size);
		}
               return (0);
	}
	return (0);
}

int open_memory_file_rd (unsigned short room_num){
    char file_name [256];
    sprintf (file_name,"/usr/local/etc/smartrobot/room%02d",room_num);
    printf ("file name %s \n",file_name);
    if ((a_file = fopen (file_name, "r")) != NULL)
	    return 1;
    return 0;
}

int open_memory_file_wr (unsigned short room_num){
    char file_name [256];
    sprintf (file_name,"/usr/local/etc/smartrobot/room%02d",room_num);
    printf ("open_memory_file_wr:file name %s \n",file_name);
    if ((a_file = fopen (file_name, "w")) != NULL)
	    return 1;
    return 0;
}

void close_memory_file (){
	if (a_file != NULL){
	   fclose (a_file);
	   a_file = NULL;
	}
}



//size_t fread(void *ptr, size_t size_of_elements, size_t number_of_elements, FILE *a_file);

//size_t fwrite(const void *ptr, size_t size_of_elements, size_t number_of_elements, FILE *a_file);
