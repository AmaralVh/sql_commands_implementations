#include "estrutura.h"

//alocacao do registro de campos
Campos *aloca_campos() {
    Campos *dados = (Campos *)calloc(1, sizeof(Campos));
    if (dados == NULL){
        erro_processamento();
    }

    return dados;
}


IndexCampos *aloca_indice() {
    IndexCampos *indexCampos = (IndexCampos *) calloc(1, sizeof(IndexCampos));

    if (indexCampos == NULL) {
        erro_processamento();
    }

    return indexCampos;
}

void realoca_indice(IndexCampos *indexCampos, int n) {
    indexCampos = (IndexCampos *) realloc(indexCampos, (n+1) * sizeof(IndexCampos));
}


//para campos variaveis alocamos char por char
void aloca_campos_variaveis(Campos *dados) {
    dados->lugarCrime = (char *)calloc(1, sizeof(char));
    dados->descricaoCrime = (char *)calloc(1, sizeof(char));
}


// alocacao de cabecalho
Cabecalho *aloca_cabecalho() {
    Cabecalho *cabecalho = (Cabecalho *)calloc(1, sizeof(Cabecalho));
    if(cabecalho == NULL) {
        erro_processamento();
    }
    return cabecalho;
}


IndexCabecalho *aloca_cabecalho_index() {
    IndexCabecalho *indexCabecalho = (IndexCabecalho *) calloc(1, sizeof(IndexCabecalho));
    if(indexCabecalho == NULL) {
        erro_processamento();
    }
    return indexCabecalho;
}


// escrevendo cabecalho no arquivo binario 
void escreve_cabecalho(FILE *arquivo, Cabecalho *cabecalho) {
    fwrite(&cabecalho->status, sizeof(char), 1, arquivo);
    fwrite(&cabecalho->proxByteOffset, sizeof(long long int), 1, arquivo);
    fwrite(&cabecalho->nroRegArq, sizeof(int), 1, arquivo);
    fwrite(&cabecalho->nroRegRem, sizeof(int), 1, arquivo);
}


// criando cabecalho
void cria_cabecalho(Cabecalho *cabecalho) {
    cabecalho->status = '0';
    cabecalho->proxByteOffset = 17; // Bytes do cabecalho.
    cabecalho->nroRegArq = 0;
    cabecalho->nroRegRem = 0;
}


void cria_cabecalho_ind(IndexCabecalho *indexCabecalho) {
    indexCabecalho->status = '0';
}


//atualizacao perante o byte offset e numero de arquivos
void atualiza_cabecalho_escrita(Cabecalho *cabecalho, int deslocamentoOffset) {
    cabecalho->proxByteOffset = cabecalho->proxByteOffset + deslocamentoOffset;
    cabecalho->nroRegArq = cabecalho->nroRegArq + 1;
}


// para a questao de fechar o escopo do cabecalho, necessita de uma ultima atualizacao necessaria
void atualiza_cabecalho_fechamento(FILE *arquivo, Cabecalho *cabecalho) {
    cabecalho->status = '1';

    fseek(arquivo, 0, SEEK_SET);

    escreve_cabecalho(arquivo, cabecalho);
}


void atualiza_cab_ind_fechamento(FILE *arquivo, IndexCabecalho *indexCabecalho) {
    indexCabecalho->status = '1';

    fseek(arquivo, 0, SEEK_SET);

    fwrite(&indexCabecalho->status, sizeof(char), 1, arquivo);
}


// abrindo arquivo
FILE *abre_arquivo(char *nome_arquivo, int tipo, Cabecalho *cabecalho) {
    FILE *arquivo;

    // fizemos um if para cada tipo, sendo o primeiro para leitura 
    if (tipo == 1) {
        arquivo = fopen(nome_arquivo, "r");
    } 
    else if (tipo == 2) { // abertura para escrever no binario
        arquivo = fopen(nome_arquivo, "wb");
        cria_cabecalho(cabecalho);
    }
    else if (tipo == 3) { // abertura para ler no binario
        arquivo = fopen(nome_arquivo, "rb");
    } 

    if(arquivo == NULL) {
        erro_processamento();
    }

    return arquivo;
}


// fechamento e atualizacao do cobacalho
void fecha_arquivo(FILE *arquivo, int tipo, Cabecalho *cabecalho) {
    if (tipo == 2){
        atualiza_cabecalho_fechamento(arquivo, cabecalho);
    }

    fclose(arquivo);
}


// Retorna 0 se o tipo do index for int, e 1 se for string:
int seleciona_tipo(char *tipoDado) {
    if(strcmp(tipoDado, "inteiro") == 0) {
        return 0;
    } else if(strcmp(tipoDado, "string") == 0) {
        return 1;
    } else {
        erro_processamento();
    }
}


void preenche_cifrao(IndexCampos *indexCampos, IndexCabecalho *indCab) {   
    printf("QUANTIDADE: %d\n", indCab->quant);

    for(int i = 0; i < indCab->quant; i++) {
        int tamanhoString = strlen(indexCampos[i].chaveStr);
        
        if(tamanhoString < 12) {
            int j;

            for(j = tamanhoString; j < 12; j++) {
                indexCampos[i].chaveStr[j] = '$';
            }
            indexCampos->chaveStr[j] = '\0';
        }
    }
}


void trunca_chaves(Campos *campos) {
    int tamLugar = strlen(campos->lugarCrime);
    int tamDescricao = strlen(campos->descricaoCrime);

    if(tamLugar > 12) {
        char lugTruncado[13];

        for(int i = 0; i < 12; i++) {
            lugTruncado[i] = campos->lugarCrime[i];
        }

        campos->lugarCrime = (char *) realloc(campos->lugarCrime, 13 * sizeof(char));
        strcpy(campos->lugarCrime, lugTruncado);
    } 
  

    if(tamDescricao > 12) {
        char descTruncada[13];

        for(int i = 0; i < 12; i++) {
            descTruncada[i] = campos->descricaoCrime[i];
        }

        campos->descricaoCrime = (char *) realloc(campos->descricaoCrime, 13 * sizeof(char));
        strcpy(campos->descricaoCrime, descTruncada);
     }
    
}


int compara_data(char *data1, char *data2) {
    char d1[11], d2[11];
    char dia1[3], dia2[3];
    char mes1[3], mes2[3];
    char ano1[5], ano2[5];

    strcpy(d1, data1);
    strcpy(d2, data2);

    // printf("COMP: data1 = %s data2 = %s\n", d1, d2);

    int igualdade = 0;

    strcpy(dia1, strtok(d1, "/"));
    strcpy(mes1, strtok(NULL, "/"));
    strcpy(ano1, strtok(NULL, "/"));
    strcpy(dia2, strtok(d2, "/"));
    strcpy(mes2, strtok(NULL, "/"));
    strcpy(ano2, strtok(NULL, "/"));

    igualdade = atoi(ano1) - atoi(ano2);
    // printf(" %s e %s ", ano1, ano2);
    if(igualdade == 0) {
        igualdade = atoi(mes1) - atoi(mes2);

        if(igualdade == 0) {
            igualdade = atoi(dia1) - atoi(dia2);
        }
    }
    
    return igualdade;
}


void shift_index(IndexCampos *indexCampos, int i, IndexCabecalho *indCab, int tipo) {
    if(tipo == 0) {
        for(int k = indCab->quant - 1; k > i; k--) {
            indexCampos[k].chaveInt = indexCampos[k-1].chaveInt;
            indexCampos[k].byteOffset = indexCampos[k-1].byteOffset;
        }
    } else if(tipo == 1) {
        for(int k = indCab->quant - 1; k > i; k--) {
            strcpy(indexCampos[k].chaveStr, indexCampos[k-1].chaveStr);
            indexCampos[k].byteOffset = indexCampos[k-1].byteOffset;
        }
    }
}


void insere_index_int(IndexCampos *indexCampos, int *ponteiro, IndexCabecalho *indCab, long long int byteOffset) {
    int i = 0;

    while(i < (indCab->quant - 1) && *ponteiro >= indexCampos[i].chaveInt) {
        i++;
    }

    if(i < indCab->quant - 1) {
        shift_index(indexCampos, i, indCab, 0);
    }

    indexCampos[i].chaveInt = *ponteiro;
    indexCampos[i].byteOffset = byteOffset;
}


void insere_index_str(IndexCampos *indexCampos, char *ponteiro, IndexCabecalho *indCab, int data, long long int byteOffset) {
    int i = 0;
    
    if(data == 0) {
        while(i < (indCab->quant - 1) && strcmp(ponteiro, indexCampos[i].chaveStr) >= 0) {
            i++;
        }
    } else {
        while(i < (indCab->quant - 1) && compara_data(ponteiro, indexCampos[i].chaveStr) >= 0) {
            i++;
        }
    }

    if(i < indCab->quant - 1) {
        shift_index(indexCampos, i, indCab, 1);
    }
    
    strcpy(indexCampos[i].chaveStr, ponteiro);
    indexCampos[i].byteOffset = byteOffset;
}


void seleciona_index(IndexCampos *indexCampos, Campos *campos, char *campoIndexado, int tipo, IndexCabecalho *indCab, long long int byteOffset) {

    // printf("Data depois de tratada: %s, ", campos->dataCrime);
    // printf("com tamanho de string: %ld.\n", strlen(campos->dataCrime));

    if(tipo == 0) {
        int *ponteiro;

        if(strcmp(campoIndexado, "idCrime") == 0) {
            ponteiro = &(campos->idCrime);
        } else if(strcmp(campoIndexado, "numeroArtigo") == 0) {
            ponteiro = &(campos->numeroArtigo);
        } else {
            erro_processamento();
        }

        if(*ponteiro != 0) {
            indCab->quant = indCab->quant + 1;
            insere_index_int(indexCampos, ponteiro, indCab, byteOffset);
        }
    } else if(tipo == 1) {
        trunca_chaves(campos);

        int data = 0;
        char *ponteiro;

        if(strcmp(campoIndexado, "dataCrime") == 0) {
            data = 1;
            ponteiro = campos->dataCrime;
        } else if(strcmp(campoIndexado, "marcaCelular") == 0) {
            ponteiro = campos->marcaCelular;
        } else if(strcmp(campoIndexado, "lugarCrime") == 0) {
            ponteiro = campos->lugarCrime;
        } else if(strcmp(campoIndexado, "descricaoCrime") == 0) {
            ponteiro = campos->descricaoCrime;
        } else {
            erro_processamento();
        }

        // printf("Data depois de ponteiro: %s, ", ponteiro);
        // printf("com tamanho de string: %ld.\n", strlen(ponteiro));
        
        if(strcmp(ponteiro, "NULO") != 0) {
            indCab->quant = indCab->quant + 1;
            insere_index_str(indexCampos, ponteiro, indCab, data, byteOffset);
        }
    }

}