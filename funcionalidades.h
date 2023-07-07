#ifndef FUNCIONALIDADES_H
#define FUNCIONALIDADES_H

#include <stdio.h>
#include <stdlib.h>
#include "arquivo_bin.h"
#include "arquivo_csv.h"
#include "arquivo_ind.h"
#include "utils.h"
#include "estrutura_index.h"


void create_table();

void select_from();

void create_index(int func);

long long int **select_from_where(int func);

void delete_from_where();

void insert_into();

void update();

void create_index_arvore_b();

#endif