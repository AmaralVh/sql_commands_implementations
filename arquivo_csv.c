#include "arquivo_csv.h"

// leitura e desperdicio do cabecalho
void le_cabecalho_csv(FILE *arquivo){
    char primeira_linha[100];

    fgets(primeira_linha, sizeof(primeira_linha), arquivo);
    fscanf(arquivo, "\n");

}

// leitura de dados de tamanho fixo 
int le_arquivo_csv(FILE *arquivo, Campos *dados){
    int leuRegistro = 0;
    int leuNumeroArtigo = 0;
    char numeroArt[10];

    if (feof(arquivo) == 0){ // Trata o caso de uma tabela vazia.
        leuRegistro = fscanf(arquivo, "%d,", &dados->idCrime);
        fscanf(arquivo, "%[^,],", dados->dataCrime);
        leuNumeroArtigo = fscanf(arquivo, "%[^,]", numeroArt);
        le_campos_variaveis_csv(arquivo, dados, 1);
        le_campos_variaveis_csv(arquivo, dados, 2);
        fscanf(arquivo, "%[^\n]\n", dados->marcaCelular);

        // Trata o numero do artigo com '-1':
        if (leuNumeroArtigo == 0){
            dados->numeroArtigo = -1;
        }
        else{
            dados->numeroArtigo = atoi(numeroArt);
        }

        // Trata a marca de celular com '$':
        int tamMarcaCelular = strlen(dados->marcaCelular);
        if (tamMarcaCelular < 12){
            int i;
            for (i = tamMarcaCelular; i < 12; i++){
                dados->marcaCelular[i] = '$';
            }
            dados->marcaCelular[i] = '\0';
        }
    }

    return leuRegistro;
}

// tratando dados para caso NULO e sobra de bytes
void trata_dados_csv(Campos *dados){
    dados->removido = '0';
    strcpy(dados->dataCrime, "$$$$$$$$$$");
    strcpy(dados->marcaCelular, "$$$$$$$$$$$$");
}

// leitura dos campos variaveis
void le_campos_variaveis_csv(FILE *arquivo, Campos *dados, int opcao){
    char c;
    int i = 0;

    switch (opcao){
    case 1:                     // leitura de lugar crime
        fscanf(arquivo, ",");
        do{
            fscanf(arquivo, "%c", &c);
            if (c != ','){
                dados->lugarCrime = (char *)realloc(dados->lugarCrime, (i + 1) * sizeof(char)); // alocacao de char em char
                dados->lugarCrime[i] = c;
                i++;
            }
        } while (c != ',');
        dados->lugarCrime = (char *)realloc(dados->lugarCrime, (i + 1) * sizeof(char));
        dados->lugarCrime[i] = '\0';    // atualizacao do ultimo char
        break;
    case 2:                     // leitura da descricao do crime
        do{
            fscanf(arquivo, "%c", &c);
            if (c != ','){
                if (i > 0){
                    dados->descricaoCrime = (char *)realloc(dados->descricaoCrime, (i + 1) * sizeof(char)); // alocacao de char em char
                }
                dados->descricaoCrime[i] = c;
                i++;
            }
        } while (c != ',');
        dados->descricaoCrime = (char *)realloc(dados->descricaoCrime, (i + 1) * sizeof(char));
        dados->descricaoCrime[i] = '\0';       // atualizacao do ultimo char
        break;
    }
}

// funcao dada
void binarioNaTela(char *nomeArquivoBinario){
    unsigned long i, cs;
    unsigned char *mb;
    size_t fl;
    FILE *fs;
    if (nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))){
        fprintf(stderr,
                "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): "
                "não foi possível abrir o arquivo que me passou para leitura. "
                "Ele existe e você tá passando o nome certo? Você lembrou de "
                "fechar ele com fclose depois de usar?\n");
        return;
    }
    fseek(fs, 0, SEEK_END);
    fl = ftell(fs);
    fseek(fs, 0, SEEK_SET);
    mb = (unsigned char *)malloc(fl);
    fread(mb, 1, fl, fs);

    cs = 0;
    for (i = 0; i < fl; i++){
        cs += (unsigned long)mb[i];
    }
    printf("%lf\n", (cs / (double)100));
    free(mb);
    fclose(fs);
}
