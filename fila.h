#include <stdbool.h> 
#include <sys/time.h>

//Necessário pois está sendo Usado na Main também 
typedef struct tipoInstrucao TipoInstrucao;
struct tipoInstrucao {
	char tipoExecIo[5]; /*exec ou Io*/
	int tempoExecIo;
	bool instrucaoLida; /*Marca para sabermos quais Instruções já foram lidas no Vetor*/

	struct timeval tempoInicialInstrucao;
	struct timeval tempoFinalInstrucao;
};
typedef struct noProcesso NoProcesso;
typedef struct fila Fila;

Fila* criarFila (); 

int pegarTamanhoFila (Fila* fila);

void inserirFila (Fila *f,
				  int idProcesso,
				  int tamanhoProcesso,
				  int quantidadeExecIo,
				  char *tipoExecIo,
				  int tempoExecIo,
				  bool instrucaoLida,
				  struct timeval tempoInicialInstrucao,
				  struct timeval tempoFinalInstrucao);

void inserirProcessoFila (Fila *f,NoProcesso *processo);

void imprimirFila(Fila *f);

void imprimirProcessoArquivoLog (NoProcesso *processo);

void imprimirFilaArquivoLog (Fila *f);
 
bool filaVazia(Fila *f);

NoProcesso* retirarProcessoDaFila(Fila *f);

int pegarTamanhoProcesso(NoProcesso *processo);

int pegarIdProcesso(NoProcesso *processo);

TipoInstrucao* pegarProximaInstrucaoNaoLida(NoProcesso *processo);

void iniciarRelogioDoProcesso(NoProcesso *processo);

void iniciarRelogioDaInstrucao(TipoInstrucao *tipoInstrucao);

void retomarRelogioDaInstrucao(TipoInstrucao *tipoInstrucao);

void incrementarRelogioDoProcesso(NoProcesso *processo);

void incrementarRelogioDaInstrucao(TipoInstrucao *tipoInstrucao);

int pegarTempoDecorridoDoProcesso(NoProcesso *processo);

int pegarTempoDecorridoDaInstrucao(TipoInstrucao tipoInstrucao);

bool instrucaoExecIoTerminou(TipoInstrucao *tipoInstrucao);

bool atingiuTempoLimite(NoProcesso *processo,int timeSlice);

void atualizarProcessosAguardandoIo(Fila *filaBloqueados,Fila *filaProntos);