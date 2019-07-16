//--------------------- 
//�Ѿ����޸ģ���лԭ���ߣ�mill_li ��
//ԭ�ģ�https://blog.csdn.net/mill_li/article/details/53174390 
//--------------------- 

#pragma once
#include "xml.h"

//���λ��
static char *out_file_path = NULL;
//�ܹ��м���tag
static int tag_number = 0;

/*
 * ������
 *      �������·��
 *
 * ������
 *      format�������ʽ
 *      �����FILE�ͣ�����������·��
 *
 * ����ֵ��
 *      �����Ƿ�ɹ�
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
 *  ������
 *      ����xmlͷ
 *  ������
 *      format:�����ʽ
 *
 *  ����ֵ��
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
 *  ������
 *      ���ø����
 *
 *  ������
 *      tag������������
 *      value����������ֵ
 *  
 *  ����ֵ��
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
 *  ������
 *      �����ӽ��
 *
 *  ������
 *      tag���ӽڵ�����
 *      value���ӽڵ�����ֵ
 *
 *  ����ֵ��
 *      none
 *
 */
void array_tag_start(char *tag, char *value)
{
    int n = 0;

    //��ǰtag�ǵ�n��tag���͸�ǰ���n-1��Tab��
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

    //��ǰtag�ǵ�n��tag���͸�ǰ���n-1��Tab��
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
 *  ������
 *      ���ñ�ǩֵ
 *
 *  ������
 *      tag����ǩ����
 *      value����ǩֵ
 *
 *  ����ֵ��
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
 *  ������
 *      �����ӽڵ������
 *
 *  ������
 *      tag���ӽڵ�����
 *
 *  ����ֵ��
 *      none
 *
 */
void array_tag_end(char *tag)
{
    int n = 0;

    //��ǰvalue�ǵ�n��tag���͸�ǰ���n��Tab��
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
    //��ǰvalue�ǵ�n��tag���͸�ǰ���n��Tab��
    for (n = 0; n < tag_number; n++) {
        strcat(out_file_path, "\t");
    }
    strcat(out_file_path, "\t");
    strcat(out_file_path, "</");
    strcat(out_file_path, tag);
    strcat(out_file_path, ">\n");
}

/*
 *  ������
 *      ���ø��ڵ������
 *
 *  ������
 *      tag�����ڵ�����
 *
 *  ����ֵ��
 *      none
 *
 */
void root_tag_end(char *tag)
{
    int n = 0;

    //��ǰvalue�ǵ�n��tag���͸�ǰ���n��Tab��
    for (n = 0; n < tag_number - 1; n++) {
        strcat(out_file_path, "\t");
    }
    strcat(out_file_path, "</");
    strcat(out_file_path, tag);
    strcat(out_file_path, ">\n");

    //�ɹ�����tag_number��һ
    tag_number -= 1;
}
