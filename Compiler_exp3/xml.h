//--------------------- 
//�Ѿ����޸ģ���лԭ���ߣ�mill_li ��
//ԭ�ģ�https://blog.csdn.net/mill_li/article/details/53174390 
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
//���ߣ�mill_li 
//��Դ��CSDN 
//ԭ�ģ�https://blog.csdn.net/mill_li/article/details/53174390 
//��Ȩ����������Ϊ����ԭ�����£�ת���븽�ϲ������ӣ�