all: utils.o estrutura.o estrutura_index.o arquivo_csv.o arquivo_bin.o arquivo_ind.o funcionalidades.o programaTrab.o 
	gcc utils.o estrutura.o estrutura_index.o arquivo_csv.o arquivo_bin.o arquivo_ind.o funcionalidades.o programaTrab.o -o programaTrab -std=c99 -Wall

utils.o:
	gcc -c utils.c -o utils.o

estrutura.o:
	gcc -c estrutura.c -o estrutura.o

estrutura_index.o:
	gcc -c estrutura_index.c -o estrutura_index.o

arquivo_csv.o:
	gcc -c arquivo_csv.c -o arquivo_csv.o

arquivo_bin.o:
	gcc -c arquivo_bin.c -o arquivo_bin.o

arquivo_ind.o:
	gcc -c arquivo_ind.c -o arquivo_ind.o

funcionalidades.o:
	gcc -c funcionalidades.c -o funcionalidades.o

programaTrab.o:
	gcc -c programaTrab.c -o programaTrab.o

clean:
	rm *.o programaTrab

run:
	./programaTrab