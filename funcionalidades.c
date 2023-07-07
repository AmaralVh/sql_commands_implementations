#include "funcionalidades.h"
#include "arquivo_bin.h"

// Escolha da primeira funcionalidade:
void create_table() {
    FILE *arquivo1, *arquivo2;
    char nome_arquivocsv[50];
    char nome_arquivobin[50];
    Campos *dados;
    Cabecalho *cabecalho;

    scanf("%s %s", nome_arquivocsv, nome_arquivobin);

    cabecalho = aloca_cabecalho();

    // Abertura de arquivos
    arquivo1 = abre_arquivo(nome_arquivocsv, 1, cabecalho);
    arquivo2 = abre_arquivo(nome_arquivobin, 2, cabecalho);

    if (arquivo1 == NULL) { // Caso arquivo nao exista
        erro_processamento();
    }
    else {
        // Alocacao de campos:
        dados = aloca_campos();

        le_cabecalho_csv(arquivo1); // Descarte de cabecalho

        aloca_campos_variaveis(dados, 0);

        // Transferindo registros do csv para bin um de cada vez:
        while (feof(arquivo1) == 0) {
            trata_dados_csv(dados);
            if (le_arquivo_csv(arquivo1, dados)) {
                escreve_arquivo_bin(arquivo2, dados, cabecalho);
            }
        }

        desaloca_campos_variaveis(dados);
        free(dados);
    }

    // Fechamento de arquivos:
    fecha_arquivo(arquivo1, 1, cabecalho);
    fecha_arquivo(arquivo2, 2, cabecalho);

    free(cabecalho);

    // Saida esperada do binario:
    binarioNaTela(nome_arquivobin);
}

// Escolha da segunda funcionalidade:
void select_from() {
    FILE *arquivo;
    char nome_arquivo_bin[50];
    Campos *dados;
    Cabecalho *cabecalho;

    scanf("%s", nome_arquivo_bin);

    // Abrindo arquivos e alocando cabecalho:
    cabecalho = aloca_cabecalho();

    arquivo = abre_arquivo(nome_arquivo_bin, 3, cabecalho);

    dados = aloca_campos();

    if (!registro_inexistente(arquivo, cabecalho)) {
        // Se existe regitro e arquivo consistente, recupera dados:
        aloca_campos_variaveis(dados, 0);

        for (int cont = 0; cont < cabecalho->nroRegArq; cont++) {
            le_arquivo_bin(arquivo, dados);
            trata_dados_binario_para_estrutura(dados);
            print_de_arquivo_bin(dados);
        }
    }

    desaloca_campos_variaveis(dados);
    free(dados);

    // Fechamento do arquivo:
    fecha_arquivo(arquivo, 3, cabecalho);

    free(cabecalho);
}

// Escolha da terceira funcionalidade:
void create_index(int func) {
    FILE *arquivoBin;
    char nomeArqBin[50];
    char campoIndexado[40];
    char tipoDado[30];
    char nomeArqIndex[50];
    Campos *campos;
    Cabecalho *cabecalho;
    IndexCabecalho *indexCab;
    IndexCampos *indexCampos;

    // Recebe entradas:
    scanf("%s %s %s %s", nomeArqBin, campoIndexado, tipoDado, nomeArqIndex);

    // Aloca cabecalho e campos para leitura do arquivo binario:
    cabecalho = aloca_cabecalho();
    campos = aloca_campos();
    aloca_campos_variaveis(campos, 0);

    // Abre arquivo binario para leitura (3):
    arquivoBin = abre_arquivo(nomeArqBin, 3, cabecalho);

    // inicalizando index
    indexCab = aloca_cabecalho_index();
    indexCampos = aloca_indice();

    // Preenche a variavel 'tipo' de acordo com o 'tipoDado' lido:
    indexCab->tipo = seleciona_tipo(tipoDado);

    indexCab->quant = 0;
    long long int byteOffset = 17;

    // verificando se registro existe
    if (!registro_inexistente(arquivoBin, cabecalho)) {

        // Se arquivo consistente e se existe registro, inicia a leitura:
        for (int cont = 0; cont < cabecalho->nroRegArq; cont++) {

            // Le registro do binario:
            int tamRegBytes = le_arquivo_bin(arquivoBin, campos);

            // Trata os dados lidos (valores nulos e cifrao):
            trata_dados_binario_para_estrutura(campos);

            if (campos->removido == '0') {
                // Realoca memoria para mais um indice (se quant > 0):
                if (indexCab->quant > 0) {
                    indexCampos = (IndexCampos *)realloc(indexCampos, (indexCab->quant + 1) * sizeof(IndexCampos));
                }

                // Seleciona o campo de indexacao e o insere no array de indices (com seu byte offset):
                seleciona_index(indexCampos, campos, campoIndexado, indexCab, byteOffset);
            }

            // Incrementa o byteOffset com o tamanho do registro atual lido:
            byteOffset = byteOffset + tamRegBytes;
        }
    }

    fecha_arquivo(arquivoBin, 3, cabecalho);

    // Preenche indice com cifrao ate 12 bytes, se tipo for string:
    if (indexCab->tipo == 1) {
        preenche_cifrao_ind(indexCampos, indexCab);
    }

    // Cria o arquivo de indice:
    cria_arq_index(nomeArqIndex, indexCampos, indexCab);

    free(cabecalho);
    free(campos);
    free(indexCampos);
    free(indexCab);

    binarioNaTela(nomeArqIndex);
}

// Escolha da quarta funcionalidade:
long long int **select_from_where(int func) {
    // O parametro func indica se a funcionalidade sera usada apenas para ela mesma,
    // ou se esta sendo chamada por outro funcionalidade. (o numero dela relaciona-se
    // exatamente com o numero da funcionalidade que a chamou, e sera 4 se for para ela mesma).

    FILE *arqBin;
    FILE *arqInd;
    char nomeArqBin[50];
    char campoIndexado[40];
    char tipoDado[30];
    char nomeArqIndex[50];
    int numBuscas;
    IndexCabecalho *indexCab;
    IndexCampos *indexCampos;
    ChavesBusca *chavesBusca;
    Cabecalho *cab;
    Campos *campos;

    // Recebe os dados dos arquivos e do indice:
    scanf("%s %s %s %s %d", nomeArqBin, campoIndexado, tipoDado, nomeArqIndex, &numBuscas);


    // Cria a matriz de byteOffsets que serao retornados da busca: 
    long long int **byteOffsets;
    byteOffsets = aloca_byteoffsets(numBuscas);


    int indexado = -1; // Guarda se em certa busca, havera busca por index.
    int numPares;      // Guarda o numero de pares campo-chav para cada busca.

    indexCab = aloca_cabecalho_index();

    // Guarda o tipo do indice (0 se int, 1 se str):
    indexCab->tipo = seleciona_tipo(tipoDado);

    // Alocacao das estruturas a serem usadas na busca:
    cab = aloca_cabecalho();
    campos = aloca_campos();
    aloca_campos_variaveis(campos, 0);
    indexCampos = aloca_indice();

    // Trazendo indices do arquivo para a RAM:
    arqInd = abre_arquivo_ind(nomeArqIndex, indexCab, 2); // Abertura para leitura apenas
    le_cab_indice(arqInd, indexCab);
    indexCampos = le_arq_indice(arqInd, indexCampos, indexCab);

    arqBin = abre_arquivo(nomeArqBin, 3, cab);

    // Loop para num de buscas:
    for (int i = 0; i < numBuscas; i++) {
        scanf("%d", &numPares);

        chavesBusca = aloca_chaves_busca(numPares); // Struct para as chaves de busca.

        // Loop para num de pares de busca:
        for (int j = 0; j < numPares; j++) {
            scanf("%s", chavesBusca[j].campoBusca);

            // Verifica se ha um campo indexado para a busca:
            if (strcmp(campoIndexado, chavesBusca[j].campoBusca) == 0) {
                indexado = j;
            }

            // Caso para campo inteiro:
            if (strcmp(chavesBusca[j].campoBusca, "idCrime") == 0 || strcmp(chavesBusca[j].campoBusca, "numeroArtigo") == 0) {
                chavesBusca[j].tipo = 0;
                scanf("%d", &chavesBusca[j].chaveInt);
            }
            else { // Caso para campo string:
                chavesBusca[j].tipo = 1;
                scan_quote_string(chavesBusca[j].chaveStr);
            }
        }

        if(func == 4) {
            printf("Resposta para a busca %d\n", i + 1);
        }


        if (indexado == -1) { // Se nao ha busca por index:
            // busca sequencial no arquivo de dados:
            byteOffsets = busca_sequencial(arqBin, campos, cab, chavesBusca, numPares, func, byteOffsets, i);
        }
        else { // Se ha busca por index:

            // Verifica se arquivo nao esta inconsistente e se ha registros:
            if (!registro_inexistente(arqBin, cab)) {
                // Busca pelo indice em busca binaria (retorna o local no vetor de indices):
                int localRegInd = busca_binaria(indexCampos, indexCab->quant, chavesBusca, indexado, campos, func);

                if (localRegInd != -1) {
                    // Se o local do indice for diferente de -1, entao busca em todos os indices iguais:
                    byteOffsets = percorre_ind_iguais(indexCampos, chavesBusca, localRegInd, indexado, arqBin, campos, numPares, func, byteOffsets, i);
                }
            }
        }

        indexado = -1; // reseta o indicador de indexacao para -1
        fseek(arqBin, 0, SEEK_SET);
    }

    fecha_arquivo(arqBin, 2, cab);

    fecha_arquivo_ind(arqInd);

    free(chavesBusca);
    desaloca_campos_variaveis(campos);
    free(campos);
    free(cab);

    free(indexCab);
    free(indexCampos);

    return byteOffsets;
}


// Escolha da quinta funcionalidade:
void delete_from_where() {
    FILE *arqBin;
    FILE *arqInd;
    char nomeArqBin[50];
    char campoIndexado[40];
    char tipoDado[30];
    char nomeArqIndex[50];
    int numBuscas;
    IndexCabecalho *indexCab;
    IndexCampos *indexCampos;
    ChavesBusca *chavesBusca;
    Cabecalho *cab;
    Campos *campos;

    // Recebe os dados dos arquivos e do indice:
    scanf("%s %s %s %s %d", nomeArqBin, campoIndexado, tipoDado, nomeArqIndex, &numBuscas);


    // Cria a matriz de byteOffsets que serao retornados da busca: 
    long long int **byteOffsets;
    byteOffsets = aloca_byteoffsets(numBuscas);

    int indexado = -1; // Guarda se em certa busca, havera busca por index.
    int numPares;      // Guarda o numero de pares campo-chav para cada busca.

    indexCab = aloca_cabecalho_index();

    // Guarda o tipo do indice (0 se int, 1 se str):
    indexCab->tipo = seleciona_tipo(tipoDado);

    // Alocacao das estruturas a serem usadas na busca:
    cab = aloca_cabecalho();
    campos = aloca_campos();
    aloca_campos_variaveis(campos, 0);
    indexCampos = aloca_indice();

    // Trazendo indices do arquivo para a RAM:
    arqInd = abre_arquivo_ind(nomeArqIndex, indexCab, 2); // Abertura para leitura apenas
    le_cab_indice(arqInd, indexCab);
    indexCampos = le_arq_indice(arqInd, indexCampos, indexCab);

    // Loop para num de buscas:
    for (int i = 0; i < numBuscas; i++) {
        scanf("%d", &numPares);

        chavesBusca = aloca_chaves_busca(numPares); // Struct para as chaves de busca.

        // Loop para num de pares de busca:
        for (int j = 0; j < numPares; j++) {
            scanf("%s", chavesBusca[j].campoBusca);

            // Verifica se ha um campo indexado para a busca:
            if (strcmp(campoIndexado, chavesBusca[j].campoBusca) == 0) {
                indexado = j;
            }

            // Caso para campo inteiro:
            if (strcmp(chavesBusca[j].campoBusca, "idCrime") == 0 || strcmp(chavesBusca[j].campoBusca, "numeroArtigo") == 0) {
                chavesBusca[j].tipo = 0;
                scanf("%d", &chavesBusca[j].chaveInt);
            }
            else { // Caso para campo string:
                chavesBusca[j].tipo = 1;
                scan_quote_string(chavesBusca[j].chaveStr);
            }
        }

        arqBin = abre_arquivo(nomeArqBin, 4, cab);  // Abre para leitura e escrita.


        if (indexado == -1) { // Se nao ha busca por index:
            byteOffsets = busca_sequencial(arqBin, campos, cab, chavesBusca, numPares, 5, byteOffsets, i);
        }
        else { // Se ha busca por index:

            // Verifica se arquivo nao esta inconsistente e se ha registros:
            if (!registro_inexistente(arqBin, cab)) {
                // Busca pelo indice em busca binaria (retorna o local no vetor de indices):
                int localRegInd = busca_binaria(indexCampos, indexCab->quant, chavesBusca, indexado, campos, 5);

                if (localRegInd != -1) {
                    // Se o local do indice for diferente de -1, entao busca em todos os indices iguais:
                    byteOffsets = percorre_ind_iguais(indexCampos, chavesBusca, localRegInd, indexado, arqBin, campos, numPares, 5, byteOffsets, i);
                }
            }
        }

        for(int k = 0; k < campos->numEncontrados; k++) {
            // marca reg como removido e apaga no index o reg
            marca_removido(arqBin, campos, byteOffsets[i][k]);
            apaga_index(indexCab, byteOffsets[i][k], indexCampos);
            cab->nroRegRem++;   // atualiza numero removido
            indexCab->quant--;  // atualiza quantidade
        }

        fecha_arquivo(arqBin, 2, cab);

        indexado = -1;
    }


    fecha_arquivo_ind(arqInd);

    free(chavesBusca);
    desaloca_campos_variaveis(campos);
    free(campos);
    free(cab);

    // Atualiza arquivo de indice com os indices removidos:
    arqInd = abre_arquivo_ind(nomeArqIndex, indexCab, 1);
    escreve_arq_ind(arqInd, indexCab, indexCampos);
    fecha_arquivo_ind(arqInd);

    binarioNaTela(nomeArqBin);   // Print do binario de dados
    binarioNaTela(nomeArqIndex); // Print do binario de indice

    free(indexCab);
    free(indexCampos);
}


// funcao de inserir (sexta funcionalidade):
void insert_into() {
    char nomeArqBin[50];
    char campoIndexado[40];
    char tipoDado[30];
    char nomeArqIndex[50];
    int numInsercao;
    IndexCabecalho *indexCab;
    IndexCampos *indexCampos;
    Campos *insercao;
    Cabecalho *cab;
    FILE *arqBin;
    FILE *arqInd;

    cab = aloca_cabecalho();

    indexCampos = aloca_indice();
    indexCab = aloca_cabecalho_index();

    // Recebe os dados dos arquivos e do indice:
    scanf("%s %s %s %s %d", nomeArqBin, campoIndexado, tipoDado, nomeArqIndex, &numInsercao);

    arqBin = abre_arquivo(nomeArqBin, 4, cab);
    le_cabecalho_bin(arqBin, cab);
    arqInd = abre_arquivo_ind(nomeArqIndex, indexCab, 2); // Abre para leitura
    le_cab_indice(arqInd, indexCab);
    indexCampos = le_arq_indice(arqInd, indexCampos, indexCab);
    
    // Guarda o tipo de dados (se int, 0; se str, 1):
    indexCab->tipo = seleciona_tipo(tipoDado);

    fecha_arquivo_ind(arqInd);

    for(int i = 0; i < numInsercao; i++) {

        insercao = aloca_campos();

        char stringLida[50];
        char lugarLido[100];
        char descricaoLida[100];

        // Entrada de dados:
        scanf("%d", &insercao->idCrime); // Le o id.
        scan_quote_string(stringLida); // Le a data.
        string_lida_para_str(stringLida, insercao, "dataCrime");
        scanf("%s", stringLida); // Le o numero do artigo.
        string_lida_para_int(stringLida, insercao);
        scan_quote_string(lugarLido); // Le o lugar.
        string_lida_para_str(lugarLido, insercao, "lugarCrime");
        scan_quote_string(descricaoLida); // Le descricao.
        string_lida_para_str(descricaoLida, insercao, "descricaoCrime");
        scan_quote_string(stringLida); // Le a marca do celular.
        string_lida_para_str(stringLida, insercao, "marcaCelular");

        // Trata os dados para o caso da string do campo fixo for menor (marcaCelular):
        trata_dados_estrutura_para_binario(insercao, 6);

        long long int byteOffset = cab->proxByteOffset;
        
        // Insere registro no arquivo de dados:
        insere_arq_dados(arqBin, insercao, cab);

        // Realoca memoria para mais um indice (se quant > 0):
        if (indexCab->quant > 0) {
            indexCampos = (IndexCampos *)realloc(indexCampos, (indexCab->quant + 1) * sizeof(IndexCampos));
        }

        // Chamada para selecao e insercao do index:
        seleciona_index(indexCampos, insercao, campoIndexado, indexCab, byteOffset);

        free(insercao);

    }

    abre_arquivo_ind(nomeArqIndex, indexCab, 1);

    escreve_arq_ind(arqInd, indexCab, indexCampos);

    free(indexCampos);
    free(indexCab);

    fecha_arquivo(arqBin, 2, cab);

    fecha_arquivo_ind(arqInd);

    free(cab);

    binarioNaTela(nomeArqBin);
    binarioNaTela(nomeArqIndex);
}


// funcao para atualizar um registro (setima funcionalidade):
void update(){
    FILE *arqBin;
    FILE *arqInd;
    char campoIndexado[40];
    char tipoDado[30];
    int numBuscas;
    int numAtu; // Guarda o numero de atualizacoes em um unico registro.
    char nomeArqBin[50];
    char nomeArqIndex[50];
    IndexCabecalho *indexCab;
    IndexCampos *indexCampos;
    Cabecalho *cab;
    Campos *campos;
    ChavesBusca *chavesBusca;
    ChavesBusca *atualizacoes;

    scanf("%s %s %s %s %d", nomeArqBin, campoIndexado, tipoDado, nomeArqIndex, &numBuscas);
    
    // Cria a matriz de byteOffsets que serao retornados da busca: 
    long long int **byteOffsets;
    byteOffsets = aloca_byteoffsets(numBuscas);

    int indexado = -1; // Guarda se em certa busca, havera busca por index.
    int atuIndexada = -1;
    int numPares;      // Guarda o numero de pares campo-chav para cada busca.

    indexCab = aloca_cabecalho_index();

    // Guarda o tipo do indice (0 se int, 1 se str):
    indexCab->tipo = seleciona_tipo(tipoDado);

    // Alocacao das estruturas a serem usadas na busca:
    cab = aloca_cabecalho();
    campos = aloca_campos();
    aloca_campos_variaveis(campos, 0);
    indexCampos = aloca_indice();

    // Trazendo indices do arquivo para a RAM:
    arqInd = abre_arquivo_ind(nomeArqIndex, indexCab, 2); // Abertura para leitura apenas
    le_cab_indice(arqInd, indexCab);
    indexCampos = le_arq_indice(arqInd, indexCampos, indexCab);
    fecha_arquivo_ind(arqInd);

    arqBin = abre_arquivo(nomeArqBin, 4, cab); // Abre para leitura e escrita.

    for (int i = 0; i < numBuscas; i++) {
        // Recebe o numero de campos a serem buscados:
        scanf("%d", &numPares);

        chavesBusca = aloca_chaves_busca(numPares); // Struct para as chaves de busca.

        // Loop para num de pares de busca:
        for (int j = 0; j < numPares; j++) {
            scanf("%s", chavesBusca[j].campoBusca);

            // Verifica se ha um campo indexado para a busca:
            if (strcmp(campoIndexado, chavesBusca[j].campoBusca) == 0) {
                indexado = j;
            }

            // Caso para campo inteiro:
            if (strcmp(chavesBusca[j].campoBusca, "idCrime") == 0 || strcmp(chavesBusca[j].campoBusca, "numeroArtigo") == 0) {
                chavesBusca[j].tipo = 0;
                scanf("%d", &chavesBusca[j].chaveInt);
            }
            else { // Caso para campo string:
                chavesBusca[j].tipo = 1;
                scan_quote_string(chavesBusca[j].chaveStr);
            }
        }

        // Recebe o numero de campos a serem atualizados:
        scanf("%d", &numAtu);

        atualizacoes = aloca_chaves_busca(numAtu); // Struct para as atualizacoes.

        for (int j = 0; j < numAtu; j++) {
            scanf("%s", atualizacoes[j].campoBusca);

            // Verifica se ha um campo indexado para a busca:
            if (strcmp(campoIndexado, atualizacoes[j].campoBusca) == 0) {
                atuIndexada = j;
            }

            // Caso para campo inteiro:
            if (strcmp(atualizacoes[j].campoBusca, "idCrime") == 0 || strcmp(atualizacoes[j].campoBusca, "numeroArtigo") == 0) {
                atualizacoes[j].tipo = 0;
                char intLido[10];
                scan_quote_string(intLido);
                string_lida_para_int_chaves(intLido, atualizacoes, j);
            }
            else { // Caso para campo string:
                atualizacoes[j].tipo = 1;
                char stringLida[100];
                scan_quote_string(stringLida);
                string_lida_para_str_chaves(stringLida, atualizacoes, j);
                trata_dados_estrutura_para_binario_chave(atualizacoes, j); // preenche marcaCelular com $ 
            }
        }

        if (indexado == -1) { // Se nao ha busca por index:
            byteOffsets = busca_sequencial(arqBin, campos, cab, chavesBusca, numPares, 5, byteOffsets, i);
        }
        else { // Se ha busca por index:

            // Verifica se arquivo nao esta inconsistente e se ha registros:
            if (!registro_inexistente(arqBin, cab)) {
                // Busca pelo indice em busca binaria (retorna o local no vetor de indices):
                int localRegInd = busca_binaria(indexCampos, indexCab->quant, chavesBusca, indexado, campos, 6);

                if (localRegInd != -1) {
                    // Se o local do indice for diferente de -1, entao busca em todos os indices iguais:
                    byteOffsets = percorre_ind_iguais(indexCampos, chavesBusca, localRegInd, indexado, arqBin, campos, numPares, 5, byteOffsets, i);
                }
            }
        }

        // Loop para tratar a atualizacao para cada registro encontrado:
        for(int k = 0; k < campos->numEncontrados; k++) {
            fseek(arqBin, byteOffsets[i][k], SEEK_SET); // Vai para o byteoffset do encontrado.

            if(indexCab->quant > 0) {
                indexCampos = (IndexCampos *) realloc(indexCampos, (indexCab->quant + 1) * sizeof(IndexCampos));
            }

            // Le o registro:
            int tamRegAtual = le_arquivo_bin(arqBin, campos);

            // Trata os dados com cifrao e -1:
            trata_dados_estrutura_para_binario(campos, 7);

            // Guarda o tamanho do registro apos a atualizacao:
            int tamRegNovo = tam_atualizacao(atualizacoes, campos, tamRegAtual, numAtu);

            // Atualiza os campos com os valores novos:
            atualiza_campos(campos, atualizacoes, numAtu);

            if(tamRegNovo <= tamRegAtual) { // Reg novo cabe no lugar do antigo.
                // Vai para o lugar do reg antigo:
                fseek(arqBin, byteOffsets[i][k], SEEK_SET);

                // Escreve o registro novo:
                escreve_arquivo_bin(arqBin, campos, cab);
                cab->nroRegArq--;
                cab->proxByteOffset = cab->proxByteOffset - tamRegNovo;

                // Completa o registro com cifrao (se necessario):
                completa_reg_cifrao(tamRegAtual, tamRegNovo, arqBin);

                // Se o campo atualizado eh um campo indexado:
                if(atuIndexada != (-1)) {
                    // Se o campo indexado foi atualizado para um NULO:
                    if(inteiro_nulo(atualizacoes[atuIndexada].chaveInt) || string_nula(atualizacoes[atuIndexada].chaveStr)) {
                        // Apaga o index antigo do vetor de indices:
                        apaga_index(indexCab, byteOffsets[i][k], indexCampos);
                        indexCab->quant--;
                    }
                    else {
                        // Se campo indexado foi atualizado para um valor nao nulo, atualiza-o:
                        atualiza_ind(indexCampos, indexCab, byteOffsets[i][k], atualizacoes, atuIndexada, byteOffsets[i][k]);
                    }
                }
                

            } else { // Reg novo nao cabe no lugar do antigo, entao insere no fim e apaga o antigo:
                // Marca o antigo como removido:
                marca_removido(arqBin, campos, byteOffsets[i][k]);
                cab->nroRegRem++;

                // Volta o valor de campos para '0' (nao removido) para escrever no fim:
                campos->removido = '0';
                
                // Atualiza o indice com o novo byteOffset:
                atualiza_ind_bOffset(indexCampos, indexCab, byteOffsets[i][k], campos, campoIndexado, cab->proxByteOffset);
                
                insere_arq_dados(arqBin, campos, cab);  // insere no final

            }

            fseek(arqBin, 0, SEEK_SET);
            escreve_cabecalho(arqBin, cab);
        }

        indexado = -1;
        atuIndexada = -1;
        fseek(arqBin, 0, SEEK_SET);
    }

    fecha_arquivo(arqBin, 2, cab);

    preenche_cifrao_ind(indexCampos, indexCab);

    // Salva os novos indices no arquivo de indices:
    arqInd = abre_arquivo_ind(nomeArqIndex, indexCab, 1);
    escreve_arq_ind(arqInd, indexCab, indexCampos);
    fecha_arquivo_ind(arqInd);

    free(indexCampos);
    free(indexCab);
    free(campos);
    free(cab);

    binarioNaTela(nomeArqBin);
    binarioNaTela(nomeArqIndex);
}