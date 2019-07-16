#pragma warning( disable : 4996 )

//--------------------- 
// 作者: duuang
// 2019-4-19
//--------------------- 

#include <iostream>
#include <string>
#include <stdio.h>
#include "xml.h"

#define BUFFER_SIZE 1005  //缓冲区大小，建议不要太小

//***********global viarables********************
int linecount = 1;  //行数计数器
int wordcount = 1;  //单词计数器
FILE *outfp;        //输出文件指针
bool already_read = false;  //暂时设了个变量避免重复读写，TODO:
//***************************************
using namespace std;

//单词类型的枚举变量（属性字的值） (typedef final state 和word type)
typedef enum FSTATE {
  identifier = 0, keyword, integer_constant, floating_constant, character_constant, string_literal, punctuator, error
}FSTATE, WDTYPE;

//
// 封装了指向缓冲区的指针的各种操作（start和end指针)
//
class CharPtr {
public:
  //初始化成员变量
  CharPtr(FILE *fp) {
    this->fp = fp;
    fpos = 0;
    current_buffer = 0;
    if (!already_read) {  //只有第一次时，将一个缓冲区大小的数据读进缓冲区0
      memset(buffer[current_buffer], -1, sizeof(buffer[current_buffer]));
      fseek(fp, 0L, SEEK_SET);
      fread(buffer[current_buffer], sizeof(char) * BUFFER_SIZE, 1, fp);
    }
    ptr_to_buffer = &buffer[current_buffer][0];
    data = *ptr_to_buffer;
    already_read = true;
  };
  //将指针++（使用对半互补缓冲区）
  CharPtr &NextChar();
  //将指针--（使用对半互补缓冲区）
  CharPtr &LastChar();
  //现在的字母在文件中的位置
  int fpos;
  //指向现在的字母的指针
  char *ptr_to_buffer;
  //现在指向的字母的值
  char data;
  //打开文件的指针
  FILE *fp;
  //static的两个缓冲区，buffer[0]和buffer[1]
  static char buffer[2][BUFFER_SIZE];
  //现在使用的缓冲区
  int current_buffer;
};

char CharPtr::buffer[2][BUFFER_SIZE] = {};  //初始化static

//
//指针++
//
CharPtr& CharPtr::NextChar() {
  //如果不是指向缓冲区末尾，直接++
  if (&buffer[current_buffer][BUFFER_SIZE - 1] - ptr_to_buffer > 0) {
    fpos++;
    ptr_to_buffer++;
    data = *ptr_to_buffer;
    return *this;
  //如果指向末尾，则将下一份数据读入另一个缓冲区，并将指针指向该缓冲区开头
  } else {
    //如果已经到达文件末尾
    if (feof(fp)) {
      fpos++;
      ptr_to_buffer++;
      data = -1;
      return *this;
    }
    current_buffer = (current_buffer == 1 ? --current_buffer : ++current_buffer);
    memset(buffer[current_buffer], -1, sizeof(buffer[current_buffer]));  //最好初始化一下，要不不好判断结束条件
    int a = fread(buffer[current_buffer], sizeof(char) * BUFFER_SIZE, 1, fp);
    fpos++;
    ptr_to_buffer = &buffer[current_buffer][0];
    data = *ptr_to_buffer;
    return *this;
  }
}

//
//指针--
//
CharPtr& CharPtr::LastChar() {
  //如果不是指向第一个，直接--
  if (&buffer[current_buffer][0] - ptr_to_buffer < 0) {
    fpos--;
    ptr_to_buffer--;
    data = *ptr_to_buffer;
    return *this;
  //如果是第一个，指针直接跳转到另一个缓冲区末尾即可
  } else {
    current_buffer = (current_buffer == 1 ? --current_buffer : ++current_buffer);
    fpos--;
    ptr_to_buffer = &buffer[current_buffer][BUFFER_SIZE - 1];
    data = *ptr_to_buffer;
    return *this;
  }
}


// 输出xml开始部分
void xmlhead_out() {
  outfp = fopen(__argv[2], "w+");
  
  char buff[4096] = { '\0' };
  if (!set_path(buff)) {
    XZ_DEBUG("set_path error");
    return;
  }
  put_head(const_cast<char *>("utf-8"));
  char start_tag[105] = "";
  strcpy(start_tag, "project name=\"");
  strcat(start_tag, __argv[1]);
  strcat(start_tag, "\"");
  root_tag_start(const_cast<char *>(start_tag), NULL);
  array_tag_start(const_cast<char *>("tokens"), const_cast<char *>(""));
  printf("%s", buff);
  fwrite(buff, (strlen(buff) * sizeof(char)), 1, outfp);
}

// 输出xml中间部分
void xmlout(CharPtr &start, CharPtr &end, WDTYPE type) {
  char buff[4096] = { '\0' };
  if (!set_path(buff)) {
    XZ_DEBUG("set_path error");
    return;
  }
  int len = end.fpos - start.fpos + 1;
  char *value = new char[len + 1];
  strncpy(value, start.ptr_to_buffer, len);
  value[len] = '\0';
  char strwordcount[55];
  itoa(wordcount, strwordcount, 10);
  char strlinecount[55];
  itoa(linecount, strlinecount, 10);

  char strtype[][105] = {"identifier", "keyword", "integer-constant", "floating-constant",  "character_constant", "string_literal", "punctuator", "error"};
  array_tag_start2(const_cast<char *>("token"), const_cast<char *>(""));
  item_tag(const_cast<char *>("number"), const_cast<char *>(strwordcount));
  item_tag(const_cast<char *>("value"), const_cast<char *>(value));
  item_tag(const_cast<char *>("type"), const_cast<char *>(strtype[type]));
  item_tag(const_cast<char *>("line"), const_cast<char *>(strlinecount));
  item_tag(const_cast<char *>("valid"), const_cast<char *>(type == WDTYPE(error) ? "false" : "true"));
  array_tag_end2(const_cast<char *>("token"));

  printf("%s", buff);
  fwrite(buff, (strlen(buff) * sizeof(char)), 1, outfp);
  wordcount++;
}

// 输出xml结尾部分
void xmltail_out() {
  char buff[4096] = { '\0' };
  if (!set_path(buff)) {
    XZ_DEBUG("set_path error");
    return;
  }
  array_tag_end(const_cast<char *>("tokens"));
  root_tag_end(const_cast<char *>("project"));

  printf("%s", buff);
  fwrite(buff, (strlen(buff) * sizeof(char)), 1, outfp);
  if (outfp != NULL) {
    fclose(outfp);
  }
}

//判断是否为关键字
bool DFA_Keyword(CharPtr &start, CharPtr &end) {
  int len = end.fpos - start.fpos + 1;
  char *buffer = new char[len + 1];
  strncpy(buffer, start.ptr_to_buffer, len);
  buffer[len] = '\0';
  char keywords[][55] = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if", "inline",
    "int", "long", "register", "restrict", "return", "short", "signed", "sizeof",
    "static", "struct", "switch", "typedef", "union", "unsigned", "void", "volatile",
    "while", "_Alignas", "_Alignof", "_Atomic", "_Bool", "_Complex", "_Generic",
    "_Imaginary", "_Noreturn", "_Static_assert", "_Thread_local" };
  for (int i = 0; i < 44; i++) {
    if (strcmp(buffer, keywords[i]) == 0) {
      return true;
    }
  }
  return false;
}

bool IfNonDigit(char c) {
  if (c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
    return true;
  return false;
}

bool IfDigit(char c) {
  if (c >= '0' && c <= '9')
    return true;
  return false;
}

//判断一个识别的标识符是否其实应该为u、U、L、u8的编码前缀
bool IfEncodingPrefix(CharPtr &start, CharPtr &end) {
  int len = end.fpos - start.fpos + 1;
  char *buffer = new char[len + 1];
  strncpy(buffer, start.ptr_to_buffer, len);
  buffer[len] = '\0';
  char match[][55] = {
    "u", "U", "L", "u8"};
  for (int i = 0; i < 4; i++) {
    if (strcmp(buffer, match[i]) == 0) {
      return true;
    }
  }
  return false;
}

FSTATE DFA_String_and_Char(CharPtr &start, CharPtr &end);

//DFA，识别标识符，也识别字符常量和字符串常量（因为涉及到编码前缀）
FSTATE DFA_Identifier(CharPtr &start, CharPtr &end) {
  end.NextChar(); //刚开始时start==end, 先将end++
  int status = 1;
  while (true) {
    if (status == 1) {
      if (IfNonDigit(end.data) || IfDigit(end.data)) {
        status = 1;
        end.NextChar();
        continue;
      } else {
        status = 2;
      }
    } else if (status == 2) {
      char currentchar = end.data;  // 缓存当前字符
      end.LastChar();
      if (DFA_Keyword(start, end)) {  //end回退到单词末尾，判断单词是否为标识符
        xmlout(start, end, WDTYPE(keyword));
        end.NextChar();
        start = end;
        return WDTYPE(keyword);
      } else if (IfEncodingPrefix(start, end) && (currentchar == '\'' || currentchar == '\"')) {
        DFA_String_and_Char(start, end);
        return WDTYPE(error);
      } else {
        xmlout(start, end, WDTYPE(identifier));
        end.NextChar();
        start = end;
        return WDTYPE(identifier);
      }
    }
  }
}


bool IfNonezeroDigit(char c) {
  if (c >= '1' && c <= '9')
    return true;
  return false;
}

bool IfOctalDigit(char c) {
  if (c >= '0' && c <= '7')
    return true;
  return false;
}

bool IfHexDigit(char c) {
  if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
    return true;
  return false;
}

//子DFA，（具体部分详见实验报告）是DFA_Integer_and_Floating_Constant的一部分，写在一起实在太多了
void SubDFA_Integer_Suffix(CharPtr &start, CharPtr &end) {
  int status = 0;
  while (true) {
    if (status == 0) {
      if (end.data == 'u' || end.data == 'U') {
        status = 1;
        end.NextChar();
      } else if (end.data == 'l' || end.data == 'L') {
        status = 5;
        end.NextChar();
      }
    } else if (status == 1) {
      if (end.data == 'l' || end.data == 'L') {
        status = 2;
        end.NextChar();
      } else {
        status = 4;
      }
    } else if (status == 2) {
      if (end.data == 'l' || end.data == 'L') {
        status = 3;
        end.NextChar();
      } else {
        status = 4;
      }
    } else if (status == 3) {
      status = 4;
    } else if (status == 5) {
      if (end.data == 'l' || end.data == 'L') {
        status = 6;
        end.NextChar();
      } else if (end.data == 'u' || end.data == 'U') {
        status = 8;
        end.NextChar();
      } else {
        status = 4;
      }
    } else if (status == 6) {
      if (end.data == 'u' || end.data == 'U') {
        status = 7;
        end.NextChar();
      } else {
        status = 4;
      }
    } else if (status == 7) {
      status = 4;
    } else if (status == 8) {
      status = 4;
    } else if (status == 4) {
      end.LastChar();
      xmlout(start, end, WDTYPE(integer_constant));
      end.NextChar();
      start = end;
      return;
    }
  }

}

//子DFA，是DFA_Integer_and_Floating_Constant的一部分，写在一起实在太多了
void SubDFA_Floating_Suffix_DEC(CharPtr &start, CharPtr &end) {
  end.NextChar();
  int status = 1;
  while (true) {
    if (status == 1) {
      if (end.data == '+' || end.data == '-') {
        status = 2;
        end.NextChar();
      } else if (IfDigit(end.data)) {
        status = 3;
        end.NextChar();
      } else {
        end.LastChar();
        xmlout(start, end, WDTYPE(error));
        end.NextChar();
        start = end;
        return;
      }
    } else if (status == 2) {
      if (IfDigit(end.data)) {
        status = 3;
        end.NextChar();
      } else {
        end.LastChar();
        xmlout(start, end, WDTYPE(error));
        end.NextChar();
        start = end;
        return;
      }
    } else if (status == 3) {
      if (IfDigit(end.data)) {
        end.NextChar();
      } else if (end.data == 'f' || end.data == 'F' || end.data == 'l' || end.data == 'L') {
        status = 4;
        end.NextChar();
      } else {
        status = 5;
      }
    } else if (status == 4) {
      status = 5;
    } else if (status == 5) {
      end.LastChar();
      xmlout(start, end, WDTYPE(floating_constant));
      end.NextChar();
      start = end;
      return;
    }
  }
}

//子DFA，是DFA_Integer_and_Floating_Constant的一部分，写在一起实在太多了
void SubDFA_Floating_Suffix_HEX(CharPtr &start, CharPtr &end) {
  end.NextChar();
  int status = 1;
  while (true) {
    if (status == 1) {
      if (end.data == '+' || end.data == '-') {
        status = 2;
        end.NextChar();
      } else if (IfHexDigit(end.data)) {
        status = 3;
        end.NextChar();
      } else {
        end.LastChar();
        xmlout(start, end, WDTYPE(error));
        end.NextChar();
        start = end;
        return;
      }
    } else if (status == 2) {
      if (IfHexDigit(end.data)) {
        status = 3;
        end.NextChar();
      } else {
        end.LastChar();
        xmlout(start, end, WDTYPE(error));
        end.NextChar();
        start = end;
        return;
      }
    } else if (status == 3) {
      if (IfHexDigit(end.data)) {
        end.NextChar();
      } else if (end.data == 'f' || end.data == 'F' || end.data == 'l' || end.data == 'L') {
        status = 4;
        end.NextChar();
      } else {
        status = 5;
      }
    } else if (status == 4) {
      status = 5;
    } else if (status == 5) {
      end.LastChar();
      xmlout(start, end, WDTYPE(floating_constant));
      end.NextChar();
      start = end;
      return;
    }
  }
}

//DFA，识别整数常量和字符串常量
//TODO: 还有改进空间。。写的太长了点，后来发现有很多分支其实可以简化。。
//不过正确性是有保障的。。测试过了
FSTATE DFA_Integer_and_Floating_Constant(CharPtr &start, CharPtr &end) {
  int status = 0;
  while (true) {
    if (status == 0) {
      if (IfNonezeroDigit(end.data)) {
        status = 1;
        end.NextChar();
      } else if (end.data == '0') {
        status = 2;
        end.NextChar();
      } else if(end.data == '.') {
        status = 11;
        end.NextChar();
      }
    } else if (status == 11) {
      if (IfDigit(end.data)) {
        status = 12;
        end.NextChar();
      } else {
        status = 13;
      }
    } else if (status == 13) {
      end.LastChar();
      xmlout(start, end, WDTYPE(punctuator));
      end.NextChar();
      start = end;
      return WDTYPE(punctuator);
    } else if (status == 12) {
      if (IfDigit(end.data)) {
        status = 12;
        end.NextChar();
      } else if (end.data == 'e' || end.data == 'E') {
        SubDFA_Floating_Suffix_DEC(start, end);
      } else if (end.data == 'f' || end.data == 'F' || end.data == 'l' || end.data == 'L') {
        status = 14;
        end.NextChar();
      } else {
        status = 9;
      }
    } else if (status == 14) {
      status = 9;

    } else if (status == 1) {
      if (IfDigit(end.data)) {
        status = 1;
        end.NextChar();
      } else if (end.data == 'u' || end.data == 'U' || end.data == 'l' || end.data == 'L') {
        SubDFA_Integer_Suffix(start, end);
        return WDTYPE(integer_constant);
      } else if (end.data == '.') {
        status = 8;
        end.NextChar();
      } else if (end.data == 'e' || end.data == 'E') {
        SubDFA_Floating_Suffix_DEC(start, end);
      } else {
        status = 6;
      }
    } else if (status == 8) {
      if (IfDigit(end.data)) {
        status = 8;
        end.NextChar();
      } else if (end.data == 'f' || end.data == 'F' || end.data == 'l' || end.data == 'L') {
        status = 10;
        end.NextChar();
      } else if (end.data == 'e' || end.data == 'E') {
        SubDFA_Floating_Suffix_DEC(start, end);
      } else {
        status = 9;
      }
    } else if (status == 10) {
      status = 9;

    } else if (status == 2) {
      if (IfOctalDigit(end.data)) {
        status = 3;
        end.NextChar();
      } else if (end.data == 'X' || end.data == 'x') {
        status = 4;
        end.NextChar();
      } else if (end.data == '8' || end.data == '9') {
        status = 15;
        end.NextChar();
      } else if (end.data == '.') {
        status = 18;
        end.NextChar();
      } else {
        status = 6;
      }
    } else if (status == 15) {
      if (IfDigit(end.data)) {
        status = 15;
        end.NextChar();
      } else if (end.data == 'e' || end.data == 'E') {
        SubDFA_Floating_Suffix_DEC(start, end);
      } else if (end.data == '.') {
        status = 16;
        end.NextChar();
      } else {
        // 089!!!!!! ------------------------error
        end.LastChar();
        xmlout(start, end, WDTYPE(error));
        end.NextChar();
        start = end;
        return WDTYPE(error);
      }
    } else if (status == 16) {
      if (IfDigit(end.data)) {
        end.NextChar();
      } else if (end.data == 'f' || end.data == 'F' || end.data == 'l' || end.data == 'L') {
        status = 17;
        end.NextChar();
      } else if (end.data == 'e' || end.data == 'E') {
        SubDFA_Floating_Suffix_DEC(start, end);
      } else {
        status = 9;
      }
    } else if (status == 17) {
      status = 9;
    } else if (status == 18) {
      if (IfDigit(end.data)) {
        end.NextChar();
      } else if (end.data == 'f' || end.data == 'F' || end.data == 'l' || end.data == 'L') {
        status = 19;
        end.NextChar();
      } else if (end.data == 'E' || end.data == 'e') {
        SubDFA_Floating_Suffix_DEC(start, end);
      } else {
        status = 9;
      }
    } else if (status == 19) {
      status = 9;
    }

    else if (status == 3) {
      if (IfOctalDigit(end.data)) {
        status = 3;
        end.NextChar();
      } else if (end.data == 'u' || end.data == 'U' || end.data == 'l' || end.data == 'L') {
        SubDFA_Integer_Suffix(start, end);
        return WDTYPE(integer_constant);
      } else if (end.data == '8' || end.data == '9') {
        status = 15;
        end.NextChar();
      } else if (end.data == '.') {
        status = 18;
        end.NextChar();
      } else {
        status = 6;
      }
    } else if (status == 4) {
      if (IfHexDigit(end.data)) {
        status = 5;
        end.NextChar();
      } else if (end.data == '.') {
        status = 20;
        end.NextChar();
      } else {  //------------------------------------------error!!
        end.LastChar();
        xmlout(start, end, WDTYPE(error));
        end.NextChar();
        start = end;
        return WDTYPE(error);
      }
    } else if (status == 20) {
      if (IfHexDigit(end.data)) {
        status = 21;
        end.NextChar();
      } else {
        //--------------------------------------error,  0x.aaaa
        end.LastChar();
        xmlout(start, end, WDTYPE(error));
        end.NextChar();
        start = end;
        return WDTYPE(error);
      }
    } else if (status == 21) {
      if (IfHexDigit(end.data)) {
        end.NextChar();
      } else if (end.data == 'p' || end.data == 'P') {
       SubDFA_Floating_Suffix_HEX(start, end);
      } else {
        //-------------------------------------error, 0x.123s
        end.LastChar();
        xmlout(start, end, WDTYPE(error));
        end.NextChar();
        start = end;
        return WDTYPE(error); 
      }
    } else if (status == 5) {
      if (IfHexDigit(end.data)) {
        status = 5;
        end.NextChar();
      } else if (end.data == 'u' || end.data == 'U' || end.data == 'l' || end.data == 'L') {
        SubDFA_Integer_Suffix(start, end);
        return WDTYPE(integer_constant);
      } else if (end.data == 'p' || end.data == 'P') {
        SubDFA_Floating_Suffix_HEX(start, end);
      } else if (end.data == '.') {
        status = 20;
        end.NextChar();
      }
      else {
        status = 6;
      }
    } else if (status == 6) {
      end.LastChar();
      xmlout(start, end, WDTYPE(integer_constant));
      end.NextChar();
      start = end;
      return WDTYPE(integer_constant);
    } else if (status == 7) {
      end.LastChar();
      xmlout(start, end, WDTYPE(error));
      end.NextChar();
      start = end;
      return WDTYPE(error);
    } else if (status == 9) {
      end.LastChar();
      xmlout(start, end, WDTYPE(floating_constant));
      end.NextChar();
      start = end;
      return WDTYPE(floating_constant);
    }
  }
}

//识别字符常量和字符串常量的DFA（带编码前缀的情况）
FSTATE DFA_String_and_Char(CharPtr &start, CharPtr &end) {
  char tmpchar = end.NextChar().data; //缓存下一个字符值，底下会用，防止调用函数时指针来回跑
  end.LastChar();
  if (end.data == '8' && tmpchar == '\'') {  //排除u8'...'的情况
    xmlout(start, end, WDTYPE(identifier));
    end.NextChar();
    start = end;
    return WDTYPE(identifier);
  } else if (tmpchar == '\'') {
    end.NextChar();
    while (true) {
      char currentchar = end.NextChar().data;
      if (currentchar == '\\') {
        char nextchar = end.NextChar().data;
        if (nextchar == '\'' || nextchar == '\"' ||
          nextchar == '?' || nextchar == '\\' ||
          nextchar == 'a' || nextchar == 'b' ||
          nextchar == 'f' || nextchar == 'n' ||
          nextchar == 'r' || nextchar == 't' ||
          nextchar == 'v') {
          continue;
        } else {
          end.LastChar();
          xmlout(start, end, WDTYPE(error));
          end.NextChar();
          start = end;
          return WDTYPE(error);
        }
      } else if (currentchar == '\'') {
        xmlout(start, end, WDTYPE(character_constant));
        end.NextChar();
        start = end;
        return WDTYPE(character_constant);
      }
    } 
  } else if (tmpchar == '\"') {
    end.NextChar();
    while (true) {
      char currentchar = end.NextChar().data;
      if (currentchar == '\\') {
        char nextchar = end.NextChar().data;
        if (nextchar == '\'' || nextchar == '\"' ||
          nextchar == '?' || nextchar == '\\' ||
          nextchar == 'a' || nextchar == 'b' ||
          nextchar == 'f' || nextchar == 'n' ||
          nextchar == 'r' || nextchar == 't' ||
          nextchar == 'v') {
          continue;
        } else {
          end.LastChar();
          xmlout(start, end, WDTYPE(error));
          end.NextChar();
          start = end;
          return WDTYPE(error);
        }
      } else if (currentchar == '\"') {
        xmlout(start, end, WDTYPE(string_literal));
        end.NextChar();
        start = end;
        return WDTYPE(string_literal);
      }
    } 
  }
}

//识别字符常量和字符串常量的DFA（不带编码前缀的情况）
FSTATE DFA_String_and_Char_No_Prefix(CharPtr &start, CharPtr &end) {
  if (end.data == '\'') {
    while (true) {
      char currentchar = end.NextChar().data;
      if (currentchar == '\\') {
        char nextchar = end.NextChar().data;
        if (nextchar == '\'' || nextchar == '\"' ||
          nextchar == '?' || nextchar == '\\' ||
          nextchar == 'a' || nextchar == 'b' ||
          nextchar == 'f' || nextchar == 'n' ||
          nextchar == 'r' || nextchar == 't' ||
          nextchar == 'v') {
          continue;
        } else {
          end.LastChar();
          xmlout(start, end, WDTYPE(error));
          end.NextChar();
          start = end;
          return WDTYPE(error);
        }
      } else if (currentchar == '\'') {
        xmlout(start, end, WDTYPE(character_constant));
        end.NextChar();
        start = end;
        return WDTYPE(character_constant);
      }
    } 
  } else if (end.data == '\"') {
    while (true) {
      char currentchar = end.NextChar().data;
      if (currentchar == '\\') {
        char nextchar = end.NextChar().data;
        if (nextchar == '\'' || nextchar == '\"' ||
          nextchar == '?' || nextchar == '\\' ||
          nextchar == 'a' || nextchar == 'b' ||
          nextchar == 'f' || nextchar == 'n' ||
          nextchar == 'r' || nextchar == 't' ||
          nextchar == 'v') {
          continue;
        } else {
          end.LastChar();
          xmlout(start, end, WDTYPE(error));
          end.NextChar();
          start = end;
          return WDTYPE(error);
        }
      } else if (currentchar == '\"') {
        xmlout(start, end, WDTYPE(string_literal));
        end.NextChar();
        start = end;
        return WDTYPE(string_literal);
      }
    } 
  }
}

//识别Punctuator的函数，用的是最长匹配
FSTATE DFA_Punctuator(CharPtr &start, CharPtr &end) {
  char punctuators[][55] = {  
    "[", "]", "(", ")", "{", "}", "->", "++", "--", "&", "*", "+", "-", "~", "!",
    "/", "%", "<<", ">>", "<", ">", "<=", ">=", "==", "!=", "^", "|", "&&", "||", "?",
    ":", ";", "...", "=", "*=", "/=", "%=", "+=", "-=", "<<=", ">>=", "&=", "^=", "|=",
    ",", "#", "##", "<:", ":>", "<%", "%>", "%:", "%:%:", "."};  // append "."
  end.NextChar();  //依次判断长度为4、3、2、1的字符串是否和所有符号匹配（因为要按最长的匹配）
  end.NextChar();
  end.NextChar();
  for (int i = 4; i >= 1; i--) {
    int len = end.fpos - start.fpos + 1;
    char *buffer = new char[len + 1];
    strncpy(buffer, start.ptr_to_buffer, len);
    buffer[len] = '\0';
    for (int i = 0; i < 54; i++) {
      if (strcmp(buffer, punctuators[i]) == 0) {
        xmlout(start, end, WDTYPE(punctuator));
        end.NextChar();
        start = end;
        return WDTYPE(punctuator);
      }
    }
    end.LastChar();
  }
  end.NextChar();
  xmlout(start, end, WDTYPE(error));
  end.NextChar();
  start = end;
  return WDTYPE(error);
}

//
//总体DFA，识别所有单词，并转化为xml输出
//
int DFA(CharPtr &start, CharPtr &end) {
  //先判断标点（已经把之后的if中的条件已经都排除了，
  //这是因为把.的识别放在了DFA_Integer_and_Floating_Constant中，如果这个DFA不放在前面的话，
  // "..."就会被识别为三个"."".""."
  if (start.data != -1 && !IfNonDigit(start.data)
    && !IfNonezeroDigit(start.data) && start.data != '0'
    && start.data != ' ' && start.data != '\n' && start.data != '\t'
    && start.data != '\'' && start.data != '\"' && start.data != '\r') {
    /*if (start.data == '.' && IfDigit(start.NextChar().data)) {
      start.LastChar();
      goto goto_floating;
    }*/
    DFA_Punctuator(start, end);
    if (end.data == -1)
      return -1;
    return 1;
  //如果第一位是字母和下划线，调用子DFA判断标识符和带前缀的字符串
  } else if (IfNonDigit(start.data)) {
    DFA_Identifier(start, end);
    if (end.data == -1)
      return -1;
    return 1;
  //如果是数字和.开头，判断是否为整形或浮点型常量
  } else if (IfNonezeroDigit(start.data) || start.data == '0' || start.data == '.') {
    DFA_Integer_and_Floating_Constant(start, end);
    if (end.data == -1)
      return -1;
    return 1;
  //如果是回车、空格、tab，则跳过，继续下一个字符
  } else if (start.data == ' ' || start.data == '\n' || start.data == '\t' || start.data == '\r') {
    if (start.data == '\n') {
      linecount++;
    }
    start.NextChar();
    end = start;
    if (end.data == -1)
      return -1;
    return 1;
  //如果是无前缀的'....'或“....”
  } else if (start.data == '\'' || start.data == '\"') {
    DFA_String_and_Char_No_Prefix(start, end);
    if (end.data == -1)
      return -1;
    return 1;
  //如果是-1字符(EOF)，函数结束
  } else if (start.data == -1) {
    return -1;
  }
  return 1;
}

//main

int main(int argc, char* argv[]) {
  //FILE *fp = fopen("C:\\Users\\cky\\source\\repos\\Compiler_exp3\\Compiler_exp3\\test2.cpp", "r");
  FILE *fp = fopen(argv[1], "r");
  //定义头尾指针
  CharPtr start(fp), end(fp);
  //输出xml开头部分
  xmlhead_out();
  //运行DFA并同时输出xml中间部分
  while (DFA(start, end) != -1) {
    ;
  }
  //输出xml结尾部分
  xmltail_out();

  printf("xml ended\n");
  if (fp != NULL)
    fclose(fp);
  system("pause");
  return 0;
}