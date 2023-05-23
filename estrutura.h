#ifndef ESTRUTURA_H
#define ESTRUTURA_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// estrutura de cabecalho do arquivo de dados
struct cabecalho {
    char status;
    long long int proxByteOffset;
    int nroRegArq;
    int nroRegRem;
};

// estrutura de campos do arquivo de dados
struct campos {
    char removido;
    int idCrime;
    int numeroArtigo;
    char dataCrime[11];
    char marcaCelular[13];
    char *lugarCrime;
    char *descricaoCrime;
    int numEncontrados;
};

// estrutura para guardar os campos usados para busca
struct chavesBusca {
    int tipo;
    char campoBusca[40];
    union {
        int chaveInt;
        char chaveStr[100];
    };
};


typedef struct campos Campos;
typedef struct cabecalho Cabecalho;
typedef struct chavesBusca ChavesBusca;


void erro_processamento();

Campos *aloca_campos();

ChavesBusca *aloca_chaves_busca(int numPares);

Cabecalho *aloca_cabecalho();

void aloca_campos_variaveis(Campos *dados, int i);

long long int **aloca_byteoffsets(int numBuscas);

void desaloca_campos_variaveis(Campos *dados);

void cria_cabecalho(Cabecalho *cabecalho);

void trata_dados_estrutura_para_binario(Campos *dados, int func);

void trata_dados_estrutura_para_binario_chave(ChavesBusca *atualizacoes, int j);

void trata_dados_binario_para_estrutura(Campos *dados);

void preenche_cifrao_chav_str(ChavesBusca *chavesBusca, int index);

void trunca_chaves(Campos *campos);

int seleciona_tipo(char *tipoDado);

void string_lida_para_int(char *stringLida, Campos *insercao);

void string_lida_para_int_chaves(char *stringLida, ChavesBusca *chaves, int j);

Campos *string_lida_para_str(char *stringLida, Campos *insercao, char *campo);

void string_lida_para_str_chaves(char *stringLida, ChavesBusca *chaves, int j);

int inteiro_nulo(int inteiro);

int string_nula(char *string);

int tam_atualizacao(ChavesBusca *atualizacoes, Campos *campos, int tamRegAtual, int numAtualiz);

void atualiza_campos(Campos *campos,ChavesBusca *atualizacoes,int numAtualiz);

Campos *copia_campos(Campos *camposOrigem);


#endif