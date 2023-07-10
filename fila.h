#include <stdbool.h> 

//Necessário pois está sendo Usado na Main também 
typedef struct tipoInstrucao TipoInstrucao;
struct tipoInstrucao {
	char tipoExecIo[5]; /*exec ou Io*/
	int tempoExecIo;
	bool instrucaoLida; /*Marca para sabermos quais Instruções já foram lidas no Vetor*/
};
typedef struct noProcesso NoProcesso;
typedef struct fila Fila;

Fila* criarFila ();

void inserirFila (Fila *f,
				  int idProcesso,
				  int tamanhoProcesso,
				  int quantidadeExecIo,
				  char *tipoExecIo,
				  int tempoExecIo);

void imprimirFila(Fila *f);
 
bool filaVazia(Fila *f);

NoProcesso* retirarProcessoDaFila(Fila *f);

int pegarTamanhoProcesso (NoProcesso *processo);

int pegarIdProcesso (NoProcesso *processo);

TipoInstrucao pegarProximaInstrucaoNaoLida(NoProcesso *processo);

void iniciarRelogioDoProcesso(NoProcesso *processo);

void incrementarRelogioDoProcesso(NoProcesso *processo);

bool instrucaoExecIoTerminou(NoProcesso *processo,int tempoExecIo);

bool atingiuTempoLimite(NoProcesso *processo,int timeSlice);


