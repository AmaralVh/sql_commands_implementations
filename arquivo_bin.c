#include "arquivo_bin.h"


int le_cabecalho_bin(FILE *arquivo, Cabecalho *cabecalho){

    // leitura do cabecalho em binario
    fread(&cabecalho->status, sizeof(char), 1, arquivo);
    fread(&cabecalho->proxByteOffset, sizeof(long long int), 1, arquivo);
    fread(&cabecalho->nroRegArq, sizeof(int), 1, arquivo);
    fread(&cabecalho->nroRegRem, sizeof(int), 1, arquivo);

    // retorna numero de arquivos no binario:
    return cabecalho->nroRegArq;
}


// Verifica consistencia ou se arquivo esta vazio:
int registro_inexistente(FILE *arquivo, Cabecalho *cabecalho) {
    int inexistente = 0;
    
    // Caso não tenha registro, quant_registros = 0 (arq vazio):
    int quant_registros = le_cabecalho_bin(arquivo, cabecalho);

    // Se arquivo vazio, ou inconsistente:
    if(quant_registros == 0 || cabecalho->status != '1') {  
        printf("Registro inexistente.\n");
        inexistente = 1;
    }

    return inexistente;
}


// leitura de campos variaveis no binario
int le_campos_variaveis_bin(FILE *arquivo, Campos *dados, int opcao, int *tamRegBytes){
    int leuCampo = 0;
    int contador = 0;
    char c;

    // caso do Lugar Crime
    if (opcao == 1){
        do{ // leitura até o |

            fread(&c, sizeof(char), 1, arquivo);   
            if (c != '|'){  // caso chegue no barramento
                if (contador > 0){
                    dados->lugarCrime = (char *)realloc(dados->lugarCrime, (contador + 1) * sizeof(char));
                }
                dados->lugarCrime[contador] = c;
                contador++;
            }
        } while (c != '|');
        dados->lugarCrime = (char *)realloc(dados->lugarCrime, (contador + 1) * sizeof(char));
        dados->lugarCrime[contador] = '\0';     // atualização do ultimo char

        // caso para Descricao do Crime
    }else{
        do{ // leitura até o |
            fread(&c, sizeof(char), 1, arquivo);

            if (c != '|'){      // caso chegue no barramento
                if (contador > 0){
                    dados->descricaoCrime = (char *)realloc(dados->descricaoCrime, (contador + 1) * sizeof(char));
                }
                dados->descricaoCrime[contador] = c;
                contador++;
            }
        } while (c != '|');
        dados->descricaoCrime = (char *)realloc(dados->descricaoCrime, (contador + 1) * sizeof(char));
        dados->descricaoCrime[contador] = '\0';     // atualização do ultimo char
    }

    if(contador > 0) {
        leuCampo = 1;
    }

    *tamRegBytes = *tamRegBytes + contador;

    // retorna se o campo existe ou nao
    return leuCampo;
}

// chamando funcao de leitura com campos variaveis e fixos
int le_arquivo_bin(FILE *arquivo, Campos *dados){
    char delimitador;
    int leuLugar;
    int leuDescricao;
    int tamRegBytes = 0;

    // leitura de Tamanho fixo
    fread(&dados->removido, sizeof(char), 1, arquivo);
    fread(&dados->idCrime, sizeof(int), 1, arquivo);
    fread(&dados->dataCrime, sizeof(char), 10, arquivo);
    fread(&dados->numeroArtigo, sizeof(int), 1, arquivo);
    fread(&dados->marcaCelular, sizeof(char), 12, arquivo);

    tamRegBytes = tamRegBytes + 31 + 3;

    // leitura de campos variaveis
    leuLugar = le_campos_variaveis_bin(arquivo, dados, 1, &tamRegBytes);
    leuDescricao = le_campos_variaveis_bin(arquivo, dados, 2, &tamRegBytes);
    fread(&delimitador, sizeof(char), 1, arquivo);

    // casos NULOS
    if(leuLugar == 0) {
        strcpy(dados->lugarCrime, "NULO");
    }
    if(leuDescricao == 0) {
        strcpy(dados->descricaoCrime, "NULO");
    }

    return tamRegBytes;
}

// tratando do caso NULO e dos casos de sobra de bytes
void trata_dados_bin(Campos *dados){
    if (strcmp(dados->dataCrime, "") == 0){
        strcpy(dados->dataCrime, "NULO");
    }
    if (dados->numeroArtigo == (-1)){
        dados->numeroArtigo = 0;        // indica NULO
    }
    if (strcmp(dados->marcaCelular, "$$$$$$$$$$$$") == 0) {
        strcpy(dados->marcaCelular, "NULO");
    }

    for(int i = 0; i < strlen(dados->marcaCelular); i++) {
        if(dados->marcaCelular[i] == '$') {
            dados->marcaCelular[i] = '\0';
            break;
        }
    }
}

// escrita no arquivo binario
void escreve_arquivo_bin(FILE *arquivo, Campos *dados, Cabecalho *cabecalho){
    if (cabecalho->nroRegArq == 0){
        escreve_cabecalho(arquivo, cabecalho);
    }

    char delimitador_campo = '|';
    char delimitador_registro = '#';

    fwrite(&dados->removido, sizeof(char), 1, arquivo);
    fwrite(&dados->idCrime, sizeof(int), 1, arquivo);
    fwrite(dados->dataCrime, sizeof(char), 10, arquivo);
    fwrite(&dados->numeroArtigo, sizeof(int), 1, arquivo);
    fwrite(dados->marcaCelular, sizeof(char), 12, arquivo);
    fwrite(dados->lugarCrime, sizeof(char), strlen(dados->lugarCrime), arquivo);
    fwrite(&delimitador_campo, sizeof(char), 1, arquivo);
    fwrite(dados->descricaoCrime, sizeof(char), strlen(dados->descricaoCrime), arquivo);
    fwrite(&delimitador_campo, sizeof(char), 1, arquivo);
    fwrite(&delimitador_registro, sizeof(char), 1, arquivo);

    // Calcula o deslocamento do byte offset de acordo com os bytes ocupados pelos campos fixos
    // (31), pelos dois campos variaveis e pelos delimitadores de campos e de registro (3):
    int deslocamentoOffset = 31 + strlen(dados->lugarCrime) + strlen(dados->descricaoCrime) + 3;

    atualiza_cabecalho_escrita(cabecalho, deslocamentoOffset);
}

// prints para a opcao 2 onde escreve todos os dados na tela
void print_de_arquivo_bin(Campos *dados){
    if(dados->removido == '0') {
        printf("%d, ", dados->idCrime);
        printf("%s, ", dados->dataCrime);

        if (dados->numeroArtigo == 0)
            printf("NULO, ");
        else
            printf("%d, ", dados->numeroArtigo);

        printf("%s, ", dados->lugarCrime);
        printf("%s, ", dados->descricaoCrime);
        printf("%s", dados->marcaCelular);
        printf("\n");
    }
}

int num_registros_bin(Cabecalho *cabecalho) {
    return cabecalho->nroRegArq;
}