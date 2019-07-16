//--------------------- 
//已经过修改，感谢原作者：mill_li ，
//原文：https://blog.csdn.net/mill_li/article/details/53174390 
//--------------------- 

#pragma once
#ifndef __XZ_XMLLIB_H__
#define __XZ_XMLLIB_H__

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

//#define _XZ_DEBUG
#ifdef _XZ_DEBUG
#define XZ_DEBUG(format, ...)   printf(format"\n", ##__VA_ARGS__)
#else
#define XZ_DEBUG(format, ...)
#endif

bool set_path(char *path);
void put_head(char *format);

void root_tag_start(char *tag, char *value);
void array_tag_start(char *tag, char *value);
void array_tag_start2(char *tag, char *value);
void item_tag(char *tag, char *value);
void array_tag_end(char *tag);
void array_tag_end2(char *tag);
void root_tag_end(char *tag);

#endif
//--------------------- 
//作者：mill_li 
//来源：CSDN 
//原文：https://blog.csdn.net/mill_li/article/details/53174390 
//版权声明：本文为博主原创文章，转载请附上博文链接！