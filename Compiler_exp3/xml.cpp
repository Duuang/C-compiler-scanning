//--------------------- 
//已经过修改，感谢原作者：mill_li ，
//原文：https://blog.csdn.net/mill_li/article/details/53174390 
//--------------------- 

#pragma once
#include "xml.h"

//输出位置
static char *out_file_path = NULL;
//总共有几个tag
static int tag_number = 0;

/*
 * 描述：
 *      设置输出路径
 *
 * 参数：
 *      format：输出格式
 *      如果是FILE型，则后面参数接路径
 *
 * 返回值：
 *      设置是否成功
 *
 */
bool set_path(char *path)
{
    bool flag = false;

    if (path != NULL) {
        out_file_path = path;
        flag = true;
    }

    return flag;
}

/*
 *
 *  描述：
 *      设置xml头
 *  参数：
 *      format:编码格式
 *
 *  返回值：
 *      none
 */
void put_head(char *format)
{
    strcat(out_file_path, "<?");
    strcat(out_file_path, "xml version=\"1.0\" encoding=");
    strcat(out_file_path, "\"");
    strcat(out_file_path, format);
    strcat(out_file_path, "\"");
    strcat(out_file_path, "?>\n");
}

/*
 *  描述：
 *      设置根结点
 *
 *  参数：
 *      tag：根结点的名称
 *      value：附加属性值
 *  
 *  返回值：
 *      none
 */
void root_tag_start(char *tag, char *value)
{
    int n = 0;

    tag_number += 1;

    strcat(out_file_path, "<");
    strcat(out_file_path, tag);
    if (value != NULL) {
        strcat(out_file_path, " ");
        strcat(out_file_path, value);
    }
    strcat(out_file_path, ">");
    strcat(out_file_path, "\n");

}

/*
 *  描述：
 *      设置子结点
 *
 *  参数：
 *      tag：子节点名称
 *      value：子节点属性值
 *
 *  返回值：
 *      none
 *
 */
void array_tag_start(char *tag, char *value)
{
    int n = 0;

    //当前tag是第n个tag，就给前面加n-1个Tab符
    for (n = 0; n < tag_number; n++) {
        strcat(out_file_path, "\t");
    }
    strcat(out_file_path, "<");
    strcat(out_file_path, tag);
    if (value != NULL) {
        //strcat(out_file_path, " ");
        strcat(out_file_path, value);
    }
    strcat(out_file_path, ">");
    strcat(out_file_path, "\n");
}

void array_tag_start2(char *tag, char *value)
{
    int n = 0;

    //当前tag是第n个tag，就给前面加n-1个Tab符
    for (n = 0; n < tag_number; n++) {
        strcat(out_file_path, "\t");
    }
    strcat(out_file_path, "\t");
    strcat(out_file_path, "<");
    strcat(out_file_path, tag);
    if (value != NULL) {
        //strcat(out_file_path, " ");
        strcat(out_file_path, value);
    }
    strcat(out_file_path, ">");
    strcat(out_file_path, "\n");
    tag_number++;
}


/*
 *  描述：
 *      设置标签值
 *
 *  参数：
 *      tag：标签名称
 *      value：标签值
 *
 *  返回值：
 *      none
 *
 */
void item_tag(char *tag, char *value)
{
    int n = 0;

    for (n = 0; n <= tag_number; n++) {
        strcat(out_file_path, "\t");
    }

    strcat(out_file_path, "<");
    strcat(out_file_path, tag);
    strcat(out_file_path, ">");
    
    strcat(out_file_path, value);

    strcat(out_file_path, "</");
    strcat(out_file_path, tag);
    strcat(out_file_path, ">");

    strcat(out_file_path, "\n");
}

/*
 *  描述：
 *      设置子节点结束符
 *
 *  参数：
 *      tag：子节点名称
 *
 *  返回值：
 *      none
 *
 */
void array_tag_end(char *tag)
{
    int n = 0;

    //当前value是第n个tag，就给前面加n个Tab符
    for (n = 0; n < tag_number; n++) {
        strcat(out_file_path, "\t");
    }
    strcat(out_file_path, "</");
    strcat(out_file_path, tag);
    strcat(out_file_path, ">\n");
}

void array_tag_end2(char *tag)
{
    int n = 0;
    tag_number--;
    //当前value是第n个tag，就给前面加n个Tab符
    for (n = 0; n < tag_number; n++) {
        strcat(out_file_path, "\t");
    }
    strcat(out_file_path, "\t");
    strcat(out_file_path, "</");
    strcat(out_file_path, tag);
    strcat(out_file_path, ">\n");
}

/*
 *  描述：
 *      设置根节点结束符
 *
 *  参数：
 *      tag：根节点名称
 *
 *  返回值：
 *      none
 *
 */
void root_tag_end(char *tag)
{
    int n = 0;

    //当前value是第n个tag，就给前面加n个Tab符
    for (n = 0; n < tag_number - 1; n++) {
        strcat(out_file_path, "\t");
    }
    strcat(out_file_path, "</");
    strcat(out_file_path, tag);
    strcat(out_file_path, ">\n");

    //成功，则tag_number减一
    tag_number -= 1;
}
