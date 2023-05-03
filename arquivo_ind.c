#include "arquivo_ind.h"
#include <stdbool.h>


void le_cab_indince(FILE *arq, IndexCabecalho *indexCabecalho) {
    fread(&indexCabecalho->status, sizeof(char), 1, arq);
    fread(&indexCabecalho->quant, sizeof(int), 1, arq);
}


// Abre o arquivo de indices:
FILE *abre_arquivo_ind(char *nomeArquivoIndex, IndexCabecalho *indexCabecalho, int tipo) {
    FILE *arquivo;

    if(tipo == 1) {
        // Abertura para escrita:
        arquivo = fopen(nomeArquivoIndex, "wb");
         // Criacao do cabecalho do arq. de indices:
        cria_cabecalho_ind(indexCabecalho);
    } else if(tipo == 2) { 
        arquivo = fopen(nomeArquivoIndex, "rb");
    }

    return arquivo;
}


// Fecha arquivo de indices:
void fecha_arquivo_ind(FILE *arquivo, IndexCabecalho *indexCabecalho) {
    atualiza_cab_ind_fechamento(arquivo, indexCabecalho);

    fclose(arquivo);
}


// Cria arquivo de index:
void cria_arq_index(char *nomeArquivoIndex, IndexCampos *indexCampos, IndexCabecalho *indexCabecalho, int tipoDado) {
    FILE *arquivo = abre_arquivo_ind(nomeArquivoIndex, indexCabecalho, 1);

    // Escreve cabecalho do arquivo de indice:
    fwrite(&indexCabecalho->status, sizeof(char), 1, arquivo);
    fwrite(&indexCabecalho->quant, sizeof(int), 1, arquivo);

    // Se tipo de dado = inteiro:
    if(tipoDado == 0) {
        for(int i = 0; i < indexCabecalho->quant; i++) { // Escrita:
            fwrite(&indexCampos[i].chaveInt, sizeof(int), 1, arquivo);
            fwrite(&indexCampos[i].byteOffset, sizeof(long long int), 1, arquivo);
        }
    } else if(tipoDado == 1) { // Se tipo de dado = string:
        for(int i = 0; i < indexCabecalho->quant; i++) { // Escrita:
            fwrite(indexCampos[i].chaveStr, sizeof(char), 12, arquivo);
            fwrite(&indexCampos[i].byteOffset, sizeof(long long int), 1, arquivo);
        }
    } else {
        erro_processamento();
    }

    fecha_arquivo_ind(arquivo, indexCabecalho);
}

// void busca_bin(IndexCampos *indexCampos, char *campoIndexado, IndexCabecalho *indexCabecalho, char *nomeArqIndex, int tipo, int chaveInt){
//     FILE *arq = abre_arquivo(nomeArqIndex, indexCabecalho, 2);

//     le_cab_indince(arq, indexCabecalho);
    
//     int bFim = indexCabecalho->quant;
//     int bMedio;
//     int achou = false;
//     int bInicio = 0;


    

//     while(achou == false) {
//         bMedio = ((bInicio + bFim) / 2);

//         if(tipo == 0) { // tipo int
//             bFim = ((indexCabecalho->quant -1 )* 12) + 5 ;
//         } else if(tipo == 1) {  // tipo string
//             bFim = ((indexCabecalho->quant -1 )* 20) + 5 ;
//         }
        
//     }





//     int total = indexCabecalho->quant;
//     int regMeio = total / 2;
//     int bMeio;
//     int bInicio = 5;

//     while(achou == false) {
//         if(regMeio % 2 == 0) {
//             bMeio = bInicio + (regMeio - 1) * 12;
//         } else {
//             bMeio = bInicio + (regMeio) * 12;
//         }
        
//         if(tipo == 0) {
//         }

//     }

    
// }

