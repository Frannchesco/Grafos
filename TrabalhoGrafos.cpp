/*
ETAPA 1 - TRABALHO PRÁTICO ALGORITMOS EM GRAFOS - 06/04/2025 
Leticia G. N. Morais e Luiz Francisco   
  
*/
 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <sstream>
#include <limits>
#include <iomanip>

using namespace std;

class Grafo {
public:
    int V, E, A, ReqN, ReqE, ReqA;
    int numVeiculos, capacidade, deposito;
    vector<list<pair<int, int>>> adj;
    vector<vector<int>> matrizAdj;
    vector<vector<int>> matrizDistancia;
	vector<vector<int>> matrizPredecessor;
    vector<bool> verticesRequeridos;

    struct ArestaRequerida {
        int origem, destino, demanda, custoServico;
    };
    struct ArcoRequerido {
        int origem, destino, demanda, custoServico;
    };

    vector<ArestaRequerida> arestasRequeridas;
    vector<ArcoRequerido> arcosRequeridos;

	Grafo(int V) : V(V), verticesRequeridos(V + 1, false) {
        adj.resize(V + 1);
        matrizAdj.assign(V + 1, vector<int>(V + 1, numeric_limits<int>::max()));
        for (int i = 1; i <= V; i++) matrizAdj[i][i] = 0;
    }

    void adicionarAresta(int origem, int destino, int peso, bool requerida = false, int demanda = 0, int custoServico = 0) {
        adj[origem].push_back({destino, peso});
        adj[destino].push_back({origem, peso});
        matrizAdj[origem][destino] = peso;
        matrizAdj[destino][origem] = peso;

        if (requerida) {
            arestasRequeridas.push_back({origem, destino, demanda, custoServico});
        }
    }

    void adicionarArco(int origem, int destino, int peso, bool requerido = false, int demanda = 0, int custoServico = 0) {
        adj[origem].push_back({destino, peso});
        matrizAdj[origem][destino] = peso;

        if (requerido) {
            arcosRequeridos.push_back({origem, destino, demanda, custoServico});
        }
    }

    void marcarVerticeRequerido(int vertice) {
        verticesRequeridos[vertice] = true;
    }
    
    int getNumVeiculos() const { return numVeiculos; }
    
    int getCapacidade() const { return capacidade; }
    
    int getDeposito() const { return deposito; }
    
    int getNumVertices() const { return V; }
    
	int getNumArestas() const { return E; }
	
	int getNumArcos() const { return A; }
	
	int getNumVerticesRequeridos() const { return ReqN; }
	
	int getNumArestasRequeridas() const { return ReqE; }
	
	int getNumArcosRequeridos() const { return ReqA; }
	
	
	double calcularDensidade(int numVertices, int numArestas, int numArcos) const {
		if (numVertices <= 1) return 0.0;

		double maxLigacoes = 1.5 * numVertices * (numVertices - 1); // (3/2)n(n - 1)
		return static_cast<double>(numArestas + numArcos) / maxLigacoes;
	}

	
	int grauMinimo() const {
		int grauMin = numeric_limits<int>::max();
		for (int i = 1; i <= V; ++i)
			grauMin = min(grauMin, static_cast<int>(adj[i].size()));
		return grauMin;
	}

	int grauMaximo() const {
		int grauMax = 0;
		for (int i = 1; i <= V; ++i)
			grauMax = max(grauMax, static_cast<int>(adj[i].size()));
		return grauMax;
	}
	
	void floydWarshall() {
		matrizDistancia = matrizAdj;
		matrizPredecessor.assign(V + 1, vector<int>(V + 1, -1));

		for (int i = 1; i <= V; ++i)
			for (int j = 1; j <= V; ++j)
				if (matrizAdj[i][j] < numeric_limits<int>::max())
					matrizPredecessor[i][j] = i;

		for (int k = 1; k <= V; ++k) {
			for (int i = 1; i <= V; ++i) {
				for (int j = 1; j <= V; ++j) {
					if (matrizDistancia[i][k] < numeric_limits<int>::max() &&
						matrizDistancia[k][j] < numeric_limits<int>::max() &&
						matrizDistancia[i][j] > matrizDistancia[i][k] + matrizDistancia[k][j]) {
						
						matrizDistancia[i][j] = matrizDistancia[i][k] + matrizDistancia[k][j];
						matrizPredecessor[i][j] = matrizPredecessor[k][j];
					}
				}
			}
		}
	}

	double calcularCaminhoMedio() const {
		double soma = 0;
		int cont = 0;

		for (int i = 1; i <= V; ++i) {
			for (int j = 1; j <= V; ++j) {
				if (i != j && matrizDistancia[i][j] < numeric_limits<int>::max()) {
					soma += matrizDistancia[i][j];
					cont++;
				}
			}
		}

		return cont == 0 ? 0 : soma / cont;
	}


	int calcularDiametro() const {
		int diametro = 0;
		for (int i = 1; i <= V; ++i) {
			for (int j = 1; j <= V; ++j) {
				if (i != j && matrizDistancia[i][j] < numeric_limits<int>::max()) {
					diametro = max(diametro, matrizDistancia[i][j]);
				}
			}
		}
		return diametro;
	}

	vector<int> calcularIntermediacao() const{
		vector<int> intermediacao(V + 1, 0);

		for (int s = 1; s <= V; ++s) {
			for (int t = 1; t <= V; ++t) {
				if (s != t && matrizPredecessor[s][t] != -1) {
					int v = matrizPredecessor[s][t];
					while (v != s && v != -1) {
						intermediacao[v]++;
						v = matrizPredecessor[s][v];
					}
				}
			}
		}

		return intermediacao;
	}
	
};

Grafo lerGrafoDeArquivo(const string& nomeArquivo) {
    ifstream arquivo(nomeArquivo);
    if (!arquivo) {
        cerr << "Erro ao abrir o arquivo!\n";
        exit(1);
    }

    string linha;
    int V = 0, E = 0, A = 0, ReqN = 0, ReqE = 0, ReqA = 0;
    int numVeiculos = 0, capacidade = 0, deposito = 0;

    while (getline(arquivo, linha)) {
        if (linha.find("#Nodes:") != string::npos) V = stoi(linha.substr(linha.find(":") + 1));
        else if (linha.find("#Edges:") != string::npos) E = stoi(linha.substr(linha.find(":") + 1));
        else if (linha.find("#Arcs:") != string::npos) A = stoi(linha.substr(linha.find(":") + 1));
        else if (linha.find("#Required N:") != string::npos) ReqN = stoi(linha.substr(linha.find(":") + 1));
        else if (linha.find("#Required E:") != string::npos) ReqE = stoi(linha.substr(linha.find(":") + 1));
        else if (linha.find("#Required A:") != string::npos) ReqA = stoi(linha.substr(linha.find(":") + 1));
        else if (linha.find("#Vehicles:") != string::npos) numVeiculos = stoi(linha.substr(linha.find(":") + 1));
        else if (linha.find("Capacity:") != string::npos) capacidade = stoi(linha.substr(linha.find(":") + 1));
        else if (linha.find("Depot Node:") != string::npos) deposito = stoi(linha.substr(linha.find(":") + 1));
    }
	
    Grafo g(V);
    
    g.V = V;
    g.E = E;
    g.A = A;
    g.ReqN = ReqN;
    g.ReqE = ReqE;
    g.ReqA = ReqA;
    g.numVeiculos = numVeiculos;
    g.capacidade = capacidade;
    g.deposito = deposito;

	arquivo.clear();
	arquivo.seekg(0);
	while (getline(arquivo, linha)) {
		if (linha.find("ReN.") != string::npos) {
			while (getline(arquivo, linha) && !linha.empty() && linha[0] == 'N') {
				istringstream iss(linha);
				string label;
				int node, demand, scost;
				iss >> label >> node >> demand >> scost;
				g.marcarVerticeRequerido(node);
			}
		} else if (linha.find("ReE.") != string::npos) {
			while (getline(arquivo, linha) && !linha.empty() && linha[0] == 'E') {
				istringstream iss(linha);
				string label;
				int from, to, tcost, demand, scost;
				iss >> label >> from >> to >> tcost >> demand >> scost;
				g.adicionarAresta(from, to, tcost, true, demand, scost);
			}
		} else if (linha.find("EDGE") != string::npos) {
			while (getline(arquivo, linha) && !linha.empty() && linha[0] == 'N') {
				istringstream iss(linha);
				string label;
				int from, to, tcost;
				iss >> label >> from >> to >> tcost;
				g.adicionarAresta(from, to, tcost);
			}
		} else if (linha.find("ReA.") != string::npos) {
			while (getline(arquivo, linha) && !linha.empty() && linha[0] == 'A') {
				istringstream iss(linha);
				string label;
				int from, to, tcost, demand, scost;
				iss >> label >> from >> to >> tcost >> demand >> scost;
				g.adicionarArco(from, to, tcost, true, demand, scost);
			}
		} else if (linha.find("ARC") != string::npos) {
			while (getline(arquivo, linha) && !linha.empty() && linha[0] == 'A') {
				istringstream iss(linha);
				string label;
				int from, to, tcost;
				iss >> label >> from >> to >> tcost;
				g.adicionarArco(from, to, tcost);
			}
		}
	}
    arquivo.close();
    return g;
}

int main() {
    vector<string> arquivos = {
        "BHW1.dat", "BHW2.dat", "BHW3.dat", "BHW4.dat", "BHW5.dat", 
        "BHW6.dat", "BHW7.dat", "BHW8.dat", "BHW9.dat", "BHW10.dat", 
        "DI-NEARP-n240-Q4k.dat", "DI-NEARP-n240-Q8k.dat", "DI-NEARP-n240-Q16k.dat", 
        "DI-NEARP-n422-Q2k.dat", "DI-NEARP-n422-Q4k.dat", "DI-NEARP-n422-Q8k.dat",
        "DI-NEARP-n422-Q16k.dat", "DI-NEARP-n442-Q2k.dat", "DI-NEARP-n442-Q4k.dat", 
        "mggdb_0.25_1.dat", "mggdb_0.25_2.dat", "mggdb_0.25_3.dat", "mggdb_0.25_4.dat",
        "mggdb_0.25_5.dat", "mggdb_0.25_6.dat", "mggdb_0.25_7.dat", "mggdb_0.25_8.dat", 
        "mggdb_0.25_9.dat", "mggdb_0.25_10.dat", "mgval_0.50_8B.dat", "mgval_0.50_8C.dat",
        "mgval_0.50_9A.dat", "mgval_0.50_9B.dat", "mgval_0.50_9C.dat", "mgval_0.50_9D.dat", 
        "mgval_0.50_10A.dat", "mgval_0.50_10B.dat", "mgval_0.50_10C.dat", "mgval_0.50_10D.dat"         
    };

    ofstream csv("resultados.csv");
    if (!csv) {
        cerr << "Erro ao criar resultados.csv\n";
        return 1;
    }

	// Cabeçalho do csv
    csv << "Arquivo,Veículos,Capacidade,Depósito,Num Vértices,Num Arestas,Num Arcos,Vértices Requeridos,Arestas Requeridas,Arcos Requeridos,"
           "Densidade,Grau_Min,Grau_Max,Caminho_Medio,Diametro\n";

    for (const auto& nomeArquivo : arquivos) {
		try {
			cout << "======= Lendo arquivo: " << nomeArquivo << " =======" << endl;

			Grafo g = lerGrafoDeArquivo(nomeArquivo);
			g.floydWarshall(); //se não fizer essa chamada, dá erro de segmentação e aí não calc nada q precisa das matrizes dist e pred

			// Coletar estatísticas
			int numVeiculos = g.getNumVeiculos();
			int capacidade = g.getCapacidade();
			int deposito = g.getDeposito();
			int V = g.getNumVertices();
			int E = g.getNumArestas();
			int A = g.getNumArcos();
			int ReqN = g.getNumVerticesRequeridos();
			int ReqE = g.getNumArestasRequeridas();
			int ReqA = g.getNumArcosRequeridos();
			double densidade = g.calcularDensidade(V, E, A);
			int grauMin = g.grauMinimo();
			int grauMax = g.grauMaximo();
			double caminhoMedio = g.calcularCaminhoMedio();
			int diametro = g.calcularDiametro();

			// Escrever no CSV
			csv << nomeArquivo << "," << numVeiculos << "," << capacidade << "," << deposito << "," 
				<< V << "," << E << "," << A << ","
				<< ReqN << "," << ReqE << "," << ReqA << ","
				<< fixed << setprecision(3)
				<< densidade << "," << grauMin << "," << grauMax << ","
				<< caminhoMedio << "," << diametro << "\n";
				
		} catch (const exception& e) {
			cerr << "Erro ao processar " << nomeArquivo << ": " << e.what() << endl;
			
		} catch (...) {
			cerr << "Erro desconhecido ao processar " << nomeArquivo << endl;
		} 
    }

    csv.close();
    cout << "Arquivo resultados.csv gerado com sucesso!\n";

    return 0;
}
