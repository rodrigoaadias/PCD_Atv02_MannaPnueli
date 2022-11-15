/// Nome dos integrantes do grupo
// Artur Auresco Damasio
// Issac Santos Romão
// Rodrigo Augusto Alves Dias

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 2048
#define N_THREADS 8

void FillGlider(int **grid)
{
    // GLIDER
    int lin = 1, col = 1;

    grid[lin][col + 1] = 1;
    grid[lin + 1][col + 2] = 1;
    grid[lin + 2][col] = 1;
    grid[lin + 2][col + 1] = 1;
    grid[lin + 2][col + 2] = 1;
}

void FillRPentonimo(int **grid)
{
    // R-pentomino
    int lin = 10;
    int col = 30;
    grid[lin][col + 1] = 1;
    grid[lin][col + 2] = 1;
    grid[lin + 1][col] = 1;
    grid[lin + 1][col + 1] = 1;
    grid[lin + 2][col + 1] = 1;
}

void PrintGrid(int **grid)
{
    int i, j;
    for (i = 0; i < 50; i++)
    {
        for (j = 0; j < 50; j++)
        {
            char c = grid[i][j] == 1 ? '*' : '.';
            printf("%c", c);
        }

        printf("\n");
    }
    printf("\n\n\n");
}

int GetAliveNeighbors(int **grid, int line, int column)
{
    int alive = 0;
    int j = 0;
    int current = 0;

    // obter índice de linha acima
    int above = line == 0 ? N - 1 : line - 1;

    // checar linha de cima
    for (j = column - 1; j <= column + 1; j++)
    {
        // checa borda infinita
        current = j;
        if (current < 0)
            current = N - 1;

        if (current > N - 1)
            current = 0;

        if (grid[above][current] == 1)
            alive++;
    }

    // obter linha abaixo
    int below = (line + 1) % N;

    // checar linha de baixo
    for (j = column - 1; j <= column + 1; j++)
    {
        // checa borda infinita
        current = j;
        if (current < 0)
            current = N - 1;

        if (current > N - 1)
            current = 0;

        if (grid[below][current] == 1)
            alive++;
    }

    // checar esquerda
    int left = column > 0 ? column - 1 : N - 1;
    if (grid[line][left] == 1)
        alive++;

    // checar direita
    int right = column < N - 1 ? column + 1 : 0;
    if (grid[line][right] == 1)
        alive++;

    return alive;
}

int GetNewState(int **grid, int line, int column)
{
    int neighbors = GetAliveNeighbors(grid, line, column);

    // A célula está viva?
    if (grid[line][column] == 1)
    {
        // mantém-se viva
        if (neighbors == 2 || neighbors == 3)
            return 1;

        // deve morrer
        return 0;
    }

    // célula está morta
    // tem vizinhos suficientes para viver?
    if (neighbors == 3)
        return 1;

    return 0;
}

int GetSurvivors(int **grid)
{
    int alive = 0;
    int i, j;
    #pragma omp parallel default(none) shared(alive,grid) private(i, j)
    
    
    #pragma omp for
    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++)
        {
        #pragma omp critical (CriticalSum)
            alive += grid[i][j];
        }
    }
    return alive;
}

void ShowGeneration(int **grid, int currentGeneration)
{
    printf("Geração %d: %d\n", currentGeneration, GetSurvivors(grid));
}

int **GetCurrentGrid(int **gridA, int **gridB, int iteration)
{
    if (iteration % 2 == 0)
        return gridA;
    else
        return gridB;
}

int **GetNextGrid(int **gridA, int **gridB, int iteration)
{
    if (iteration % 2 == 0)
        return gridB;
    else
        return gridA;
}

void PlayGameOfLife(int **gridA, int **gridB, int iterations)
{
    int i, j, k;
    int th_id;
    int print_thread_num = 0;

    for (k = 0; k < iterations; k++)
    {
        int **nextGrid = GetNextGrid(gridA, gridB, k);
        int **currentGrid = GetCurrentGrid(gridA, gridB, k);

        /*if (k < 5)
            PrintGrid(currentGrid);*/

#pragma omp parallel default(none) shared(nextGrid, currentGrid, print_thread_num) private(i, j, th_id)
        {
            th_id = omp_get_thread_num();
            if (th_id == 0 && print_thread_num == 0)
            {
                printf("Numero de threads: %d\n\n", omp_get_num_threads());
                print_thread_num = 1;
            }

#pragma omp for
            for (i = 0; i < N; i++)
            {
                for (j = 0; j < N; j++)
                {
                    nextGrid[i][j] = GetNewState(currentGrid, i, j);
                }
            }
        }
    }
}

int main()
{
    int **gridA, **gridB;
    gridA = (int **)malloc(N * sizeof(int *));
    gridB = (int **)malloc(N * sizeof(int *));

    double start, end;

    int i = 0, j = 0;
    for (i = 0; i < N; i++)
    {
        gridA[i] = (int *)malloc(N * sizeof(int));
        gridB[i] = (int *)malloc(N * sizeof(int));

        for (j = 0; j < N; j++)
        {
            gridA[i][j] = 0;
            gridB[i][j] = 0;
        }
    }

    FillGlider(gridA);
    FillRPentonimo(gridA);

    printf("*** Game of Life (OPEN MP)\n");
    printf("Condição inicial: %d\n", GetSurvivors(gridA));

    start = omp_get_wtime();
    PlayGameOfLife(gridA, gridB, 2001);
    end = omp_get_wtime();
    
    double survivorStart, survivorEnd;
    
    survivorStart = omp_get_wtime();
    
    printf("Última geração (2000 iterações): %d\n", GetSurvivors(gridB));
    
    survivorEnd = omp_get_wtime();
    
    printf("Tempo execução trecho de teste: %f\n", survivorEnd - survivorStart);
    printf("Tempo execução: %f\n", end - start);

    return 0;
}
