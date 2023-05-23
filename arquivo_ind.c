#include "arquivo_ind.h"
#include <stdbool.h>


// Abre o arquivo de indices:
FILE *abre_arquivo_ind(char *nomeArquivoIndex, IndexCabecalho *indexCabecalho, int tipo) {
    FILE *arquivo;

    if(tipo == 1) {
        // Abertura para escrita:
        arquivo = fopen(nomeArquivoIndex, "wb");
         // Criacao do cabecalho do arq. de indices:
        cria_cabecalho_ind(indexCabecalho); // Status = 0
    } else if(tipo == 2) { // Abertura para leitura:
        arquivo = fopen(nomeArquivoIndex, "rb");
    }

    if(arquivo == NULL) {
        erro_processamento();
    }

    return arquivo;
}


// Escreve os indices do vetor no arquivo de indice:
void escreve_arq_ind(FILE *arquivo, IndexCabecalho *indexCabecalho, IndexCampos *indexCampos) {
    atualiza_cab_ind_fechamento(arquivo, indexCabecalho);

    // Se tipo de dado = inteiro:
    if(indexCabecalho->tipo == 0) {
        for(int i = 0; i < indexCabecalho->quant; i++) { // Escrita:
            fwrite(&indexCampos[i].chaveInt, sizeof(int), 1, arquivo);
            fwrite(&indexCampos[i].byteOffset, sizeof(long long int), 1, arquivo);
        }
    } else if(indexCabecalho->tipo == 1) { // Se tipo de dado = string:
        for(int i = 0; i < indexCabecalho->quant; i++) { // Escrita:
            fwrite(indexCampos[i].chaveStr, sizeof(char), 12, arquivo);
            fwrite(&indexCampos[i].byteOffset, sizeof(long long int), 1, arquivo);
        }
    } else {
        erro_processamento();
    }
}


// Fecha arquivo de indices:
void fecha_arquivo_ind(FILE *arquivo) {
    fclose(arquivo);
}


// Cria arquivo de index:
void cria_arq_index(char *nomeArquivoIndex, IndexCampos *indexCampos, IndexCabecalho *indexCabecalho) {
    FILE *arquivo = abre_arquivo_ind(nomeArquivoIndex, indexCabecalho, 1);

    escreve_arq_ind(arquivo, indexCabecalho, indexCampos);

    fecha_arquivo_ind(arquivo);
}


// Le o cabecalho dos indices:
void le_cab_indice(FILE *arquivo, IndexCabecalho *indexCabecalho) {
    fread(&indexCabecalho->status, sizeof(char), 1, arquivo);
    fread(&indexCabecalho->quant, sizeof(int), 1, arquivo);

    if(indexCabecalho->status == '0') {
        erro_processamento();
    }
}


// Le os indices do arquivo de indice e guarda-os no vetor em RAM:
IndexCampos *le_arq_indice(FILE *arquivo, IndexCampos *indexCampos, IndexCabecalho *indexCab) {
    indexCampos = (IndexCampos *) realloc(indexCampos, (indexCab->quant) * sizeof(IndexCampos));

    for(int i = 0; i < indexCab->quant; i++) {
        if(indexCab->tipo == 0) { // para indices inteiros
            fread(&indexCampos[i].chaveInt, sizeof(int), 1, arquivo);
            fread(&indexCampos[i].byteOffset, sizeof(long long int), 1, arquivo);
        } else if(indexCab->tipo == 1) { // para indices string
            fread(indexCampos[i].chaveStr, sizeof(char), 12, arquivo);
            fread(&indexCampos[i].byteOffset, sizeof(long long int), 1, arquivo);
        }
    }

    return indexCampos;
}
