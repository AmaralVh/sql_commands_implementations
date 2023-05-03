#ifndef ARQUIVO_BIN_H
#define ARQUIVO_BIN_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "estrutura.h"

int le_cabecalho_bin(FILE *arquivo, Cabecalho *cabecalho);

void erro_reg_inexistente();

int registro_inexistente(FILE *arquivo, Cabecalho *cabecalho);

int le_arquivo_bin(FILE *arquivo, Campos *dados);

void trata_dados_bin(Campos *dados);

int le_campos_variaveis_bin(FILE *arquivo, Campos *dados, int opcao, int *tamRegBytes);

void escreve_arquivo_bin(FILE *arquivo, Campos *dados, Cabecalho *cabecalho);

void print_de_arquivo_bin(Campos *dados, int quantidade);

int num_registros_bin(Cabecalho *cabecalho);

void binarioNaTela(char *nomeArquivoBinario);

void busca_sequencial(FILE *arquivoBin, Campos *campos, Campos *encontrados, Cabecalho *cab, ChavesBusca *chavesBusca, int numPares);

#endif