#include <raylib.h>
#include <stdlib.h>
#include "map_parser.h"

#define XOR_KEY 0x5A

char *data;
int start_pos = 0;

void LoadMapData(Map *map)
{
    map->level++;
    map->row = 1;
    map->column = 0;

    if(map->level==1)
    {
        data = LoadFileText("resources/level-map/map.bin");
        int data_len = TextLength(data);
        for(int i=0; i<data_len; i++)
            data[i] ^= XOR_KEY;
    }
    
    const char *sub_data = TextSubtext(data, start_pos, TextFindIndex(data+start_pos, "E")-1);

    int sub_data_len = TextLength(sub_data);
    start_pos += sub_data_len+3;

    for(int i=0; i<sub_data_len; i++)
    {
		map->column++;
        if(sub_data[i]=='\n')
		{
			map->column = 0;
			map->row++;
		}
    }
    if(map->level==1)
        map->data = malloc(map->row*map->column);
    else
        map->data = realloc(map->data, map->row*map->column);
    if(map->data==NULL) return;

    int c = 0;
    for(int i=0; i<sub_data_len; i++)
        if(sub_data[i]!='\n')
            map->data[c++] = sub_data[i];
}

void UnloadMapData(Map *map)
{
    UnloadFileText(data);
    if(map->data!=NULL)
        free(map->data);
    else
        fprintf(stderr, "ERROR: Could not load level due to insufficient storage. Terminating application.");
}
