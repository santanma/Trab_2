#include <stdbool.h>
#include "fila.h"

typedef struct lista Lista;
typedef struct noMemoria NoMemoria;

Lista* criarListaCPU();

bool listaCPUVazia(Lista *listaCPU);

int pegarTamanhoCPU();

int pegarTimeSliceCPU();

NoMemoria* localizarParticaoExecutandoNaCPU(Lista *listaCPU);

void insereListaCPUOrdenada(Lista *listaCPU,NoMemoria *particaoNova);

void imprimirListaCPU(Lista *listaCPU);

NoMemoria* buscarParticaoMemoriaBestFit(Lista *listaCPU,NoProcesso *processo);

NoMemoria* buscarParticaoMemoriaFirstFit(Lista *listaCPU,NoProcesso *processo);

NoMemoria* buscarParticaoMemoriaWorstFit(Lista *listaCPU,NoProcesso *processo);

void alocarMemoria(Lista *listaCPU,NoProcesso *processo,char tipoAlgoritmo);
