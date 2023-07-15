#include <stdbool.h>
#include "fila.h"

typedef struct lista Lista;
typedef struct noMemoria NoMemoria;

Lista* criarListaCPU();

bool listaCPUVazia(Lista *listaCPU);

Lista* limparCPU ();

int pegarTamanhoCPU();

int pegarTimeSliceCPU(); 

int pegarIdProcessoExecutandoNaCPU(Lista *listaCPU);

NoMemoria* localizarParticaoExecutandoNaCPU(Lista *listaCPU);

void insereListaCPUOrdenada(Lista *listaCPU,NoMemoria *particaoNova);

void imprimirListaCPU(Lista *listaCPU);

void imprimirListaCPUArquivoLog(Lista *listaCPU);

NoMemoria* buscarParticaoMemoriaBestFit(Lista *listaCPU,NoProcesso *processo);

NoMemoria* buscarParticaoMemoriaFirstFit(Lista *listaCPU,NoProcesso *processo);

NoMemoria* buscarParticaoMemoriaWorstFit(Lista *listaCPU,NoProcesso *processo);

bool alocarMemoria(Lista *listaCPU,NoProcesso *processo,char tipoAlgoritmo);
