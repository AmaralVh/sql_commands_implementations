#include "estrutura.h"

// Alocacao do registro de campos
Campos *aloca_campos() {
    Campos *dados = (Campos *)calloc(1, sizeof(Campos));
    if (dados == NULL){
        erro_processamento();
    }
    return dados;
}

// Aloca a estrutura usada para chaves de busca e atualizacoes:
ChavesBusca *aloca_chaves_busca(int numPares) {
    ChavesBusca *chavesBusca = (ChavesBusca *) calloc(numPares, sizeof(ChavesBusca));
    if (chavesBusca == NULL){
        erro_processamento();
    }
    return chavesBusca;
}

// Para campos variaveis alocamos char por char
void aloca_campos_variaveis(Campos *dados, int i) {
    dados[i].lugarCrime = (char *)calloc(1, sizeof(char));
    dados[i].descricaoCrime = (char *)calloc(1, sizeof(char));
}


// Aloca a matriz de byteoffsets:
long long int **aloca_byteoffsets(int numBuscas) {
    long long int **byteOffsets;

    byteOffsets = (long long int **) calloc(numBuscas, sizeof(long long int *));

    for(int i = 0; i < numBuscas; i++) {
        byteOffsets[i] = (long long int *) calloc(1, sizeof(long long int));
    }

    return byteOffsets;
}


// Desaloca campos variaveis:
void desaloca_campos_variaveis(Campos *dados) {
    free(dados->lugarCrime);
    free(dados->descricaoCrime);
}


// Alocacao de cabecalho
Cabecalho *aloca_cabecalho() {
    Cabecalho *cabecalho = (Cabecalho *)calloc(1, sizeof(Cabecalho));
    if(cabecalho == NULL) {
        erro_processamento();
    }
    return cabecalho;
}


// Criando cabecalho:
void cria_cabecalho(Cabecalho *cabecalho) {
    cabecalho->status = '0';
    cabecalho->proxByteOffset = 17; // Bytes do cabecalho.
    cabecalho->nroRegArq = 0;
    cabecalho->nroRegRem = 0;
}


// Retorna tipo do dado
int seleciona_tipo(char *tipoDado) {
    if(strcmp(tipoDado, "inteiro") == 0) {
        return 0;   // inteiro
    } else if(strcmp(tipoDado, "string") == 0) {
        return 1;   // sting
    } else {
        erro_processamento();
    }

    return -1;
}


// tratando dados se for NULO e sobra de bytes (Preenche cifrao):
void trata_dados_estrutura_para_binario(Campos *dados, int func){
    dados->removido = '0';

    // Preenche com cifrao caso valor ser NULO:
    if(strcmp(dados->dataCrime, "NULO") == 0) {
        strcpy(dados->dataCrime, "$$$$$$$$$$");
    }
    if(strcmp(dados->marcaCelular, "NULO") == 0) {
        strcpy(dados->marcaCelular, "$$$$$$$$$$$$");
    }
    if(dados->numeroArtigo == 0) {
        dados->numeroArtigo = -1;
    }

    if(func == 7) {
        if(strcmp(dados->lugarCrime, "NULO") == 0) {
            strcpy(dados->lugarCrime, "");
        }
        if(strcmp(dados->descricaoCrime, "NULO") == 0) {
            strcpy(dados->descricaoCrime, "");
        }
    }

    // Preenche com cifrao a marcaCelular:
    int tam = strlen(dados->marcaCelular);

    if(tam < 12) {
        for(int i = tam; i < 12; i++) {
            dados->marcaCelular[i] = '$';
        }
    }
}


// Trata os dados inserindo cifrao na marcaCelular se for nulo ou menor que 12 bytes:
void trata_dados_estrutura_para_binario_chave(ChavesBusca *atualizacoes, int j) {
    // Preenche com cifrao a marcaCelular:
    if(strcmp(atualizacoes[j].campoBusca, "marcaCelular") == 0) {
        int tam = strlen(atualizacoes[j].chaveStr);

        if(tam < 12) {
            for(int i = tam; i < 12; i++) {
                atualizacoes[j].chaveStr[i] = '$';
            }
        }
    }
}


// Tratando do caso NULO e dos casos de sobra de bytes:
void trata_dados_binario_para_estrutura(Campos *dados) {

    // caso de entrada vazia
    if(strcmp(dados->dataCrime, "") == 0) { 
        strcpy(dados->dataCrime, "NULO");
    }
    // caso de entrada nula
    if(dados->numeroArtigo == (-1)) {
        dados->numeroArtigo = 0;        // Indica NULO
    }

    // marca do celular $
    if(strcmp(dados->marcaCelular, "$$$$$$$$$$$$") == 0) {
        strcpy(dados->marcaCelular, "NULO");
    }

    //laco para colocar \0 no final da string
    for(int i = 0; i < strlen(dados->marcaCelular); i++) {
        if(dados->marcaCelular[i] == '$') {
            dados->marcaCelular[i] = '\0';
            break;
        }
    }
}


// Preenche qualquer chave str com cifrao ate 12 bytes:
void preenche_cifrao_chav_str(ChavesBusca *chavesBusca, int index) {   
    int tamanhoString = strlen(chavesBusca[index].chaveStr);
    
    // Se tam < 12, preenchemos com '$':
    if(tamanhoString < 12) {
        int j;

        for(j = tamanhoString; j < 12; j++) {
            chavesBusca[index].chaveStr[j] = '$';
        }
        chavesBusca[index].chaveStr[j] = '\0';
    } 
}


// Trunca chaves de indices se tam > 12:
void trunca_chaves(Campos *campos) {
    int tamLugar = strlen(campos->lugarCrime);
    int tamDescricao = strlen(campos->descricaoCrime);

    // Trata 'lugarCrime':
    if(tamLugar > 12) {
        char lugTruncado[13];

        for(int i = 0; i < 12; i++) {
            lugTruncado[i] = campos->lugarCrime[i];
        }

        campos->lugarCrime = (char *) realloc(campos->lugarCrime, 13 * sizeof(char));
        strcpy(campos->lugarCrime, lugTruncado);
    } 
  
    // Trata 'descricaoCrime':
    if(tamDescricao > 12) {
        char descTruncada[13];

        for(int i = 0; i < 12; i++) {
            descTruncada[i] = campos->descricaoCrime[i];
        }

        campos->descricaoCrime = (char *) realloc(campos->descricaoCrime, 13 * sizeof(char));
        strcpy(campos->descricaoCrime, descTruncada);
     }
    
}


// Processa o dado lido de uma string para um inteiro (se nulo -> -1):
void string_lida_para_int(char *stringLida, Campos *insercao) {
    if(strcmp(stringLida, "") == 0) {
        insercao->numeroArtigo = -1;
    }
    else {
        insercao->numeroArtigo = atoi(stringLida);
    }
}

// Processaoo chave lida de uma string para um inteiro (se nulo -> -1): 
void string_lida_para_int_chaves(char *stringLida, ChavesBusca *chaves, int j) {
    if(strcmp(stringLida, "") == 0) {
        chaves[j].chaveInt = -1;
    }
    else {
        chaves[j].chaveInt = atoi(stringLida);
    }
}


// Processa dado lido de string para uma string colocando cifroes se for nula:
Campos *string_lida_para_str(char *stringLida, Campos *insercao, char *campo) {
    // Preenche a string caso seja nula:
    if(strcmp(stringLida, "") == 0) {
        if(strcmp(campo, "dataCrime") == 0) {
            strcpy(insercao->dataCrime, "$$$$$$$$$$");
        } 
        else if(strcmp(campo, "marcaCelular") == 0) {
            strcpy(insercao->marcaCelular, "$$$$$$$$$$$$");
        }
        else if(strcmp(campo, "lugarCrime") == 0) {
            insercao->lugarCrime = stringLida;
        }
        else if(strcmp(campo, "descricaoCrime") == 0) {
            insercao->descricaoCrime = stringLida;
        }
    } 
    else {
        if(strcmp(campo, "dataCrime") == 0) {
            strcpy(insercao->dataCrime, stringLida);
        } 
        else if(strcmp(campo, "marcaCelular") == 0) {
            strcpy(insercao->marcaCelular, stringLida);
        }
        else if(strcmp(campo, "lugarCrime") == 0) {
            insercao->lugarCrime = stringLida;
        }
        else if(strcmp(campo, "descricaoCrime") == 0) {
            insercao->descricaoCrime = stringLida;
        }
    }

    return insercao;
}


// Processa chave lida de string para uma string colocando cifroes se for nula:
void string_lida_para_str_chaves(char *stringLida, ChavesBusca *chaves, int j) {
    // Preenche a string caso seja nula:
    if(strcmp(stringLida, "") == 0) {
        if(strcmp(chaves[j].campoBusca, "dataCrime") == 0) {
            strcpy(chaves[j].chaveStr, "$$$$$$$$$$");
        } 
        else if(strcmp(chaves[j].campoBusca, "marcaCelular") == 0) {
            strcpy(chaves[j].chaveStr, "$$$$$$$$$$$$");
        }
        else if(strcmp(chaves[j].campoBusca, "lugarCrime") == 0) {
            strcpy(chaves[j].chaveStr, stringLida);
        }
        else if(strcmp(chaves[j].campoBusca, "descricaoCrime") == 0) {
            strcpy(chaves[j].chaveStr, stringLida);
        }
    } 
    else {
        if(strcmp(chaves[j].campoBusca, "dataCrime") == 0) {
            strcpy(chaves[j].chaveStr, stringLida);
        } 
        else if(strcmp(chaves[j].campoBusca, "marcaCelular") == 0) {
            strcpy(chaves[j].chaveStr, stringLida);
        }
        else if(strcmp(chaves[j].campoBusca, "lugarCrime") == 0) {
            strcpy(chaves[j].chaveStr, stringLida);
        }
        else if(strcmp(chaves[j].campoBusca, "descricaoCrime") == 0) {
            strcpy(chaves[j].chaveStr, stringLida);
        }
    }
}


// verifica se o inteiro simbolisa nulo
int inteiro_nulo(int inteiro) {
    if(inteiro == 0 || inteiro == -1) { // 0 ou -1
        return 1;
    }
    return 0;
}


// verifica se a string simboliza nulo
int string_nula(char *string) {
    if(strcmp(string, "NULO") == 0 || strcmp(string, "") == 0) {
        return 1;
    }

    if(strcmp(string, "$$$$$$$$$$") == 0 || strcmp(string, "$$$$$$$$$$$$") == 0) {
        return 1;
    }

    return 0;
}

// contagem de bytes no registro incluindo as atualizacoes
int tam_atualizacao(ChavesBusca *atualizacoes, Campos *campos, int tamRegAtual, int numAtualiz) {
    int tamRegNovo = 0;

    int atualizaLugar = 0;
    int atualizaDesc = 0;

    // Checa se alguma atualizacao contempla descricao ou lugar do crime (se sim, inclui seus bytes na contagem):
    for(int i = 0; i < numAtualiz; i++) {
        if(strcmp(atualizacoes[i].campoBusca, "descricaoCrime") == 0) {
            tamRegNovo = tamRegNovo + strlen(atualizacoes[i].chaveStr);
            atualizaDesc = 1;

        } else if(strcmp(atualizacoes[i].campoBusca, "lugarCrime") == 0) {
            tamRegNovo = tamRegNovo + strlen(atualizacoes[i].chaveStr);
            atualizaLugar = 1;
        }
    }

    // Conta os bytes dos campos fixos e dos delimitadores no tamanho total:
    tamRegNovo = tamRegNovo + 31 + 3;

    if(atualizaLugar == 0) {
        tamRegNovo = tamRegNovo + strlen(campos->lugarCrime);   // byte do Reg antigo do lugar
    }
    if(atualizaDesc == 0) {
        tamRegNovo = tamRegNovo + strlen(campos->descricaoCrime);   // byte do Reg antigo da descricao
    }

    return tamRegNovo;
}


// Atualiza os campos para os valores novos:
void atualiza_campos(Campos *campos, ChavesBusca *atualizacoes, int numAtualiz) {
    // Para cada atualizacao, checa qual o campo e modifica-o com o novo valor:
    for(int i = 0; i < numAtualiz; i++) {

        if(strcmp(atualizacoes[i].campoBusca, "idCrime") == 0) {
            campos->idCrime = atualizacoes[i].chaveInt;

        } else if(strcmp(atualizacoes[i].campoBusca, "lugarCrime") == 0) {
            campos->lugarCrime = (char *) realloc(campos->lugarCrime, (strlen(atualizacoes[i].chaveStr)+1) * sizeof(char));
            strcpy(campos->lugarCrime, atualizacoes[i].chaveStr);
        
        } else if(strcmp(atualizacoes[i].campoBusca, "descricaoCrime") == 0) {
            campos->descricaoCrime = (char *) realloc(campos->descricaoCrime, (strlen(atualizacoes[i].chaveStr)+1) * sizeof(char));
            strcpy(campos->descricaoCrime, atualizacoes[i].chaveStr);
        
        } else if(strcmp(atualizacoes[i].campoBusca, "dataCrime") == 0) {
            strcpy(campos->dataCrime, atualizacoes[i].chaveStr);
        
        } else if(strcmp(atualizacoes[i].campoBusca, "numeroArtigo") == 0) {
            campos->numeroArtigo = atualizacoes[i].chaveInt;
        
        } else if(strcmp(atualizacoes[i].campoBusca, "marcaCelular") == 0) {
            strcpy(campos->marcaCelular, atualizacoes[i].chaveStr);

        }
        
    }
}


// Copia um registro para outro novo e o retorna:
Campos *copia_campos(Campos *camposOrigem) {
    // Aloca novos campos:
    Campos *camposDestino = aloca_campos();
    aloca_campos_variaveis(camposDestino, 0);

    // Copia de campos fixos:
    camposDestino->idCrime = camposOrigem->idCrime;
    camposDestino->numeroArtigo = camposOrigem->numeroArtigo;
    strcpy(camposDestino->dataCrime, camposOrigem->dataCrime);
    strcpy(camposDestino->marcaCelular, camposOrigem->marcaCelular);
    
    // Copia de campos variaveis, incluindo a realocacao de bytes:
    camposDestino->lugarCrime = (char *) realloc(camposDestino->lugarCrime, (strlen(camposOrigem->lugarCrime) + 1) * sizeof(char));
    camposDestino->descricaoCrime = (char *) realloc(camposDestino->descricaoCrime, (strlen(camposOrigem->descricaoCrime) + 1) * sizeof(char));

    strcpy(camposDestino->lugarCrime, camposOrigem->lugarCrime);
    strcpy(camposDestino->descricaoCrime, camposOrigem->descricaoCrime);

    return camposDestino;
}