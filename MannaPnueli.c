#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <windows.h>

#define N_CLIENTES 4
#define USE_CRITICAL_SECTION 0

// variável global de SOMA
int SOMA = 0;

// variáveis de controle de seção crítica
int request = 0;
int respond = 0;

void critical_section()
{
    int local = SOMA;
    Sleep(rand() % 2000);
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
        critical_section();

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
#pragma omp parallel num_threads(N_CLIENTES + 1)
    {
        int t = omp_get_thread_num();

        if (t == 0)
            server();
        else
            client(t);
    }
}