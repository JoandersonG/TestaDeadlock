#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

void escreverEmArquivo(char * bufferConteudoOrigem, char* arquivoDestino, char * programaInvocador) {

    //limpando dados do arquivo de destino
    fclose(fopen(arquivoDestino, "w"));

    //abrir arquivo destino:
    //flags
    //O_WRONLY: apenas escrever no arquivo
    //O_CREAT: criar se arquivo não existe
    //S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH: são flags para permitir leitura e escrita do novo arquivo por qualquer um
    int fdDestino = open(arquivoDestino, O_WRONLY| O_CREAT);
    if (fdDestino == -1) {
        if(errno == EEXIST) {
            printf("%s: não foi possível abrir o arquivo %s. Erro: o arquivo especificado já existe.\n", programaInvocador, arquivoDestino);
        } else if (errno == ENOENT) {
            printf("%s: não foi possível abrir o arquivo %s. Erro: Diretório não encontrado.\n", programaInvocador, arquivoDestino);
        } else if (errno == EACCES) {
            printf("%s: não foi possível abrir o arquivo %s. Erro: Acesso negado.\n", programaInvocador, arquivoDestino);
        } else if (errno == EAGAIN) {
            printf("%s: não foi possível abrir o arquivo %s. Tente novamente.\n", programaInvocador, arquivoDestino);
        }else {
            printf("%s: não foi possível abrir o arquivo %s. %s.\n", programaInvocador, arquivoDestino, strerror(errno));
        }
        exit(EXIT_FAILURE);
    }

    //escrever no arquivo de destino:
    int qtdBytesEscrita = write(fdDestino, bufferConteudoOrigem, strlen(bufferConteudoOrigem));
    if(qtdBytesEscrita  == -1) {
        if(errno == EPERM) {
            printf("%s: não foi possível escrever no arquivo %s. Erro: Operação não permitida.\n", programaInvocador, arquivoDestino);
        }  else if (errno == EAGAIN) {
            printf("%s: não foi possível escrever o arquivo %s. Tente novamente.\n", programaInvocador, arquivoDestino);
        }   else if (errno == ENOMEM) {
            printf("%s: não foi possível escrever o arquivo %s. Erro: memória insuficiente.\n", programaInvocador, arquivoDestino);
        } else {
            printf("%s: não foi possível escrever o arquivo %s: %s.\n", programaInvocador, arquivoDestino, strerror(errno));
        }
        close(fdDestino);
        exit(EXIT_FAILURE);
    }

    //fechar aqruivo:
    close(fdDestino);
}