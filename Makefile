all: estrutura.o arquivo_csv.o arquivo_bin.o funcionalidades.o programaTrab.o 
	gcc estrutura.o arquivo_csv.o arquivo_bin.o funcionalidades.o programaTrab.o -o programaTrab -std=c99 -Wall

estrutura.o:
	gcc -c estrutura.c -o estrutura.o

arquivo_csv.o:
	gcc -c arquivo_csv.c -o arquivo_csv.o

arquivo_bin.o:
	gcc -c arquivo_bin.c -o arquivo_bin.o

funcionalidades.o:
	gcc -c funcionalidades.c -o funcionalidades.o

programaTrab.o:
	gcc -c programaTrab.c -o programaTrab.o

clean:
	rm *.o programaTrab

run:
	./programaTrab