#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define USE_CRITICAL_SECTION 0

// variável global de SOMA
int SOMA = 0;

// variáveis de controle de seção crítica
int request = 0;
int respond = 0;

void Somar()
{
    int local = SOMA;
    SOMA = local + 1;

    printf("SOMA: %d\n", SOMA);
}

void client(int n_thread)
{
    while (1)
    {

        if (USE_CRITICAL_SECTION)
        {
            while (respond != n_thread)
                request = n_thread;
        }

        // seção crítica
        Somar();

        respond = 0;
    }
}

void server()
{
    while (1)
    {
        while (request == 0)
        {
        }

        respond = request;
        while (respond != 0)
        {
        }

        request = 0;
    }
}

int main()
{
#pragma omp parallel
    {
        int t = omp_get_thread_num();

        if (t == 0)
        {
            printf("Quantidade de Clientes: %d\n", omp_get_num_threads()-1);
            server();
        }
        else
            client(t);
    }
}