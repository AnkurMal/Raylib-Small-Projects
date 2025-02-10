#ifndef MAP_PARSER_H
#define MAP_PARSER_H

#include <stdio.h>

typedef struct Map
{
    int row;
    int column;
    int level;
    char *data;
}Map;

void LoadMapData(Map *map);
void UnloadMapData(Map *map);

#endif