#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void erro_processamento();

int *aloca_v_int();

void realoca_v_int(int *vetor, int n);

char **aloca_m_char();

void realoca_m_char(char **matriz, int n);

void scan_quote_string(char *str);


#endif