#include "funcoes.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

char nomes[1000][50];
int indice = 0;

void salva_nomes(){
    FILE *fp;
    fp = fopen("nomes.bin", "rb");
    if(fp == NULL){
        fp = fopen("nomes.bin", "wb");
    }
    while (fgets(nomes[indice], 50, fp)) {
        nomes[indice][strlen(nomes[indice]) - 1] = '\0';
        indice++;
    }
    fclose(fp);
    menu();
}

void menu() {
    printf("\nBanco QPT\n");
    printf("1 - Novo cliente\n");
    printf("2 - Apaga cliente\n");
    printf("3 - Débito\n");
    printf("4 - Depósito\n");
    printf("5 - Extrato\n");
    printf("6 - Transferência entre contas\n");
    printf("7 - Listar clientes\n");
    printf("0 - Sair\n");
    int escolha;
    scanf("%d", &escolha);

    if (escolha == 1) {
        novo();
    } else if (escolha == 2) {
        apaga();
    } else if (escolha == 3) {
        debito();
    } else if (escolha == 4) {
        deposito();
    } else if (escolha == 5) {
        extrato();
    } else if (escolha == 6) {
        transferencia();
    } else if (escolha == 7) {
        listar();
    } else if (escolha == 0) {
        sair();
    } else {
        printf("\n\nDigite uma opção válida\n");
        menu();
    }
}

void novo() {
    printf("\nNovo cliente\n\n");
    char cpf[20];
    printf("CPF: ");
    scanf("%s", cpf);
    char filename[20];
    sprintf(filename, "%s.bin", cpf);
    FILE *fp;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        char nome[50], senha[50], tip[2];
        float valor;
        printf("Nome: ");
        scanf("%s", nome);
        printf("Senha: ");
        scanf("%s", senha);
        printf("Digite o tipo de conta: 1-Comum 2-Plus. ");
        scanf("%s", tip);
        printf("Digite o valor inicial da conta: ");
        scanf("%f", &valor);
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        char tipo[6];
        if (strcmp(tip, "1") == 0) {
            strcpy(tipo, "comum");
        }
        else {
            strcpy(tipo, "plus");
        }
        fp = fopen(filename, "w");
        fprintf(fp, "%s\n%s\n%s\n%s\n", nome, cpf, senha, tipo);
        fprintf(fp, "Data: %02d-%02d-%04d %02d:%02d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min);
        fprintf(fp, "   + %8.2f   Tarifa:     0.00   Saldo: %8.2f\n", valor, valor);
        fclose(fp);
        strcpy(nomes[indice], nome);
        indice++;
        printf("Conta cadastrada com sucesso.\n");
        menu();
    }
    else{
        fclose(fp);
        printf("CPF em conta existente.\n");
        menu();
    }
}

void apaga() {
    printf("\nApaga cliente\n\n");
    char cpf[20];
    printf("CPF: ");
    scanf("%s", cpf);
    char apaga[30];
    sprintf(apaga, "%s.bin", cpf);
    FILE *fp;
    fp = fopen(apaga, "r");
    if (fp == NULL) {
        printf("Não há conta com esse CPF\n");
        menu();
    }
    else {
        fclose(fp);
        char senha[20];
        printf("Senha: ");
        scanf("%s", senha);
        FILE *arquivo = fopen(apaga, "r");
        char escrito[2][20];
        int i = 0;
        while (fgets(escrito[i], 20, arquivo)) {
            escrito[i][strlen(escrito[i]) - 1] = '\0';
            i++;
        }
        fclose(arquivo);
        if (strcmp(senha, escrito[2]) == 0) {
            remove(apaga);
            printf("%s", escrito[0]);
            for(int i=0;i<indice;i++) {
                if (strcmp(nomes[i], escrito[0]) == 0) {
                    while (i < indice) {
                        strcpy(nomes[i], nomes[i + 1]);
                        i++;
                        printf("Okk");
                    }
                }
            }
            indice--;
            printf("Confirmado\n");
            menu();
        }
        else {
            printf("CPF ou senha incorretos\n");
            menu();
        }
    }
}

void debito() {
    printf("\nDébito\n\n");
    char cpf[30];
    printf("CPF: ");
    scanf("%s", cpf);
    char filename[30];
    sprintf(filename, "%s.bin", cpf);
    FILE *fp;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Não há conta com esse CPF\n");
        menu();
    } else {
        fclose(fp);
        char senha[30];
        printf("Senha: ");
        scanf("%s", senha);
        FILE *arquivo;
        arquivo = fopen(filename, "r");
        char escrito[100][100];
        int i = 0;
        while (fgets(escrito[i], 100, arquivo)) {
            escrito[i][strlen(escrito[i]) - 1] = '\0';
            i++;
        }
        fclose(arquivo);
        int retorno = strcmp(senha, escrito[2]);
        if (retorno == 0) {
            float valor;
            printf("Digite o valor a ser debitado: ");
            scanf("%f", &valor);
            if (strcmp(escrito[3], "Tipo: comum") == 0) {
                float tarifa = valor * 0.05;
                float saldo = atof(strrchr(escrito[i-1], ' ') + 1) - valor - tarifa;
                if (saldo > -1000) {
                    time_t t = time(NULL);
                    struct tm tm = *localtime(&t);
                    arquivo = fopen(filename, "a");
                    fprintf(arquivo, "Data: %02d-%02d-%04d %02d:%02d   - %8.2f   Tarifa: %8.2f   Saldo: %8.2f\n",
                            tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, valor, tarifa, saldo);
                    fclose(arquivo);
                    printf("Confirmado\n");
                    menu();
                } else {
                    printf("Limite de saldo negativo excedido por R$ %.2f.\n", saldo + 1000);
                    printf("Operação cancelada.\n");
                    menu();
                }
            } else {
                float tarifa = valor * 0.03;
                float saldo = atof(strrchr(escrito[i - 1], ' ') + 1) - valor - tarifa;
                if (saldo > -5000) {
                    time_t t = time(NULL);
                    struct tm tm = *localtime(&t);
                    arquivo = fopen(filename, "a");
                    fprintf(arquivo, "Data: %02d-%02d-%04d %02d:%02d   - %8.2f   Tarifa: %8.2f   Saldo: %8.2f\n",
                            tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, valor, tarifa, saldo);
                    fclose(arquivo);
                    printf("Confirmado\n");
                    menu();
                } else {
                    printf("Limite de saldo negativo excedido por R$ %.2f.\n", saldo + 1000);
                    printf("Operação cancelada.\n");
                    menu();
                }
            }
        }
        else{
            printf("CPF ou senha incorretos\n");
            menu();
        }
    }
}


void deposito() {
    printf("\nDeposito\n\n");
    char cpf[20];
    printf("CPF: ");
    scanf("%s", cpf);
    char filename[30];
    sprintf(filename, "%s.bin", cpf);
    FILE *fp;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Não há conta com esse CPF\n");
        menu();
    }
    else {
        float valor;
        printf("Digite o valor a ser depositado: ");
        scanf("%f", &valor);
        FILE *arquivo;
        arquivo = fopen(filename, "r");
        char escrito[100][100];
        int i = 0;
        while (fgets(escrito[i], 100, arquivo)) {
            escrito[i][strlen(escrito[i]) - 1] = '\0';
            i++;
        }
        fclose(arquivo);
        float saldo = atof(strrchr(escrito[i - 1], ' ') + 1) + valor;
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        arquivo = fopen(filename, "a");
        fprintf(arquivo, "Data: %02d-%02d-%04d %02d:%02d   + %8.2f   Tarifa:     0.00   Saldo: %8.2f\n",
                tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, valor, saldo);
        fclose(arquivo);
        printf("Confirmado\n");
        menu();
    }
}

void extrato() {
    printf("\nExtrato\n\n");
    char cpf[30];
    printf("CPF: ");
    scanf("%s", cpf);
    char filename[30];
    sprintf(filename, "%s.bin", cpf);
    FILE *fp;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Não há conta com esse CPF\n");
        menu();
    } else {
        fclose(fp);
        char senha[30];
        printf("Senha: ");
        scanf("%s", senha);
        FILE *arquivo;
        arquivo = fopen(filename, "r");
        char escrito[100][100];
        int i = 0;
        while (fgets(escrito[i], 100, arquivo)) {
            escrito[i][strlen(escrito[i]) - 1] = '\0';
            i++;
        }
        fclose(arquivo);
        int retorno = strcmp(senha, escrito[2]);
        if (retorno == 0) {
            arquivo = fopen("extrato.txt", "w");
            fprintf(arquivo, "\n\nNome: %s\n", escrito[0]);
            fprintf(arquivo, "CPF: %s\n", escrito[1]);
            fprintf(arquivo, "Tipo: %s\n", escrito[3]);
            for(int x=4; x<i; x++){
                fprintf(arquivo, "%s\n", escrito[x]);
            }
            printf("\n\nNome: %s\n", escrito[0]);
            printf("CPF: %s\n", escrito[1]);
            printf("Tipo: %s\n", escrito[3]);
            for(int y=4; y<i; y++){
                printf("%s\n", escrito[y]);
            }
            printf("\nArquivo de extrato gerado com sucesso com o nome extrato.txt");
            menu();
        }
        else{
            printf("CPF ou senha incorretos\n");
            menu();
        }
    }
}

void transferencia() {
    printf("\nTranferencia\n\n");
    char cpf[30];
    printf("CPF da pessoa origem: ");
    scanf("%s", cpf);
    char filename[30];
    sprintf(filename, "%s.bin", cpf);
    FILE *fp;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Não há conta com esse CPF\n");
        menu();
    } else {
        fclose(fp);
        char senha[30];
        printf("Senha da pessoa origem: ");
        scanf("%s", senha);
        FILE *arquivo;
        arquivo = fopen(filename, "r");
        char escrito[100][100];
        int i = 0;
        while (fgets(escrito[i], 100, arquivo)) {
            escrito[i][strlen(escrito[i]) - 1] = '\0';
            i++;
        }
        fclose(arquivo);
        int retorno = strcmp(senha, escrito[2]);
        if (retorno == 0) {
            char cpf2[30];
            printf("CPF da pessoa destino: ");
            scanf("%s", cpf2);
            char filename2[30];
            sprintf(filename2, "%s.bin", cpf2);
            FILE *fp2;
            fp2 = fopen(filename2, "r");
            if (fp2 == NULL) {
                printf("Não há conta com esse CPF\n");
                menu();
            } else {
                char escritod[100][100];
                int x = 0;
                while (fgets(escritod[i], 100, fp2)) {
                    escritod[i][strlen(escritod[i]) - 1] = '\0';
                    x++;
                }
                fclose(fp2);
                float valor;
                printf("Digite o valor a ser transferido: ");
                scanf("%f", &valor);
                if (strcmp(escrito[3], "Tipo: comum") == 0) {
                    float tarifa = valor * 0.05;
                    float saldoo = atof(strrchr(escrito[i-1], ' ') + 1) - valor - tarifa;
                    printf("%f", atof(strrchr(escrito[i - 1], ' ') + 1));
                    printf("%f", valor);
                    printf("%f", tarifa);
                    printf("%f", saldoo);
                    if (saldoo > -1000) {
                        time_t t = time(NULL);
                        struct tm tm = *localtime(&t);
                        arquivo = fopen(filename, "a");
                        fprintf(arquivo, "Data: %02d-%02d-%04d %02d:%02d   - %8.2f   Tarifa: %8.2f   Saldo: %8.2f\n",
                                tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, valor, tarifa, saldoo);
                        fclose(arquivo);
                        float saldod = atof(strrchr(escritod[x-1], ' ') + 1) + valor;
                        fp2 = fopen(filename2, "a");
                        fprintf(fp2, "Data: %02d-%02d-%04d %02d:%02d   + %8.2f   Tarifa:     0.00   Saldo: %8.2f\n",
                                tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, valor, saldod);
                        fclose(fp2);
                        printf("Confirmado\n");
                        menu();
                    } else {
                        printf("Limite de saldo negativo excedido por R$ %.2f.\n", saldoo + 1000);
                        printf("Operação cancelada.\n");
                        menu();
                    }
                } else {
                    float tarifa = valor * 0.03;
                    float saldoo = atof(strrchr(escrito[i - 1], ' ') + 1) - valor - tarifa;
                    printf("%f", atof(strrchr(escrito[i - 1], ' ') + 1));
                    printf("%f", valor);
                    printf("%f", tarifa);
                    printf("%f", saldoo);
                    if (saldoo > -5000) {
                        time_t t = time(NULL);
                        struct tm tm = *localtime(&t);
                        arquivo = fopen(filename, "a");
                        fprintf(arquivo, "Data: %02d-%02d-%04d %02d:%02d   - %8.2f   Tarifa: %8.2f   Saldo: %8.2f\n",
                                tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, valor, tarifa, saldoo);
                        fclose(arquivo);
                        float saldod = atof(strrchr(escritod[x-1], ' ') + 1) + valor;
                        fp2 = fopen(filename2, "a");
                        fprintf(fp2, "Data: %02d-%02d-%04d %02d:%02d   + %8.2f   Tarifa:     0.00   Saldo: %8.2f\n",
                                tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, valor, saldod);
                        fclose(fp2);
                        printf("Confirmado\n");
                        menu();
                    } else {
                        printf("Limite de saldo negativo excedido por R$ %.2f.\n", saldoo + 1000);
                        printf("Operação cancelada.\n");
                        menu();
                    }
                }
            }
        }
        else{
            printf("CPF ou senha da pessoa origem incorretos\n");
            menu();
        }
    }
}

void listar() {
    printf("\nNossos clientes:\n\n");
    for(int x=0;x<indice;x++){
        printf("%s\n", nomes[x]);
    }
    menu();
}

void sair() {
    FILE *fp;
    fp = fopen("nomes.bin", "wb");
    for(int x=0;x<indice;x++){
        fprintf(fp, "%s\n", nomes[x]);
    }
    fclose(fp);
    printf("\nSaindo...\n");
    exit(0);
}
