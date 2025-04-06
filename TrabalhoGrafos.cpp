/*
ETAPA 1 - TRABALHO PRÁTICO ALGORITMOS EM GRAFOS - 06/04/2025 
Leticia G. N. Morais e Luiz Francisco S. de Jesus
  
#problemas atuais: marcação dos vértices requeridos  
  
*/
 
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <sstream>
#include <limits>

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

    void imprimirGrafo() { //só imprime a lista de adjacência, pq algumas matrizes são gigantescas
        
        cout << "\nLista de Adjacência:\n";
        for (int i = 1; i <= V; ++i) {
            cout << i << ": ";
            for (auto& viz : adj[i]) {
                cout << "(" << viz.first << ", " << viz.second << ") ";
            }
            cout << endl;
        }
        cout << endl;
    }
    
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
	
	void imprimirEstatisticas() const {
		cout << "\n====== Estatísticas do Grafo ======\n";
		cout << "1. Número de vértices: " << getNumVertices() << endl;
		cout << "2. Número de arestas: " << getNumArestas() << endl;
		cout << "3. Número de arcos: " << getNumArcos() << endl;
		cout << "4. Número de vértices requeridos: " << getNumVerticesRequeridos() << endl;
		cout << "5. Número de arestas requeridas: " << getNumArestasRequeridas() << endl;
		cout << "6. Número de arcos requeridos: " << getNumArcosRequeridos() << endl;
		cout << "7. Densidade do grafo: " << calcularDensidade(getNumVertices(), getNumArestas(), getNumArcos()) << endl;
		cout << "8. Grau mínimo: " << grauMinimo() << endl;
		cout << "9. Grau máximo: " << grauMaximo() << endl;
		cout << "10. Caminho médio: " << calcularCaminhoMedio() << endl;
		cout << "11. Diâmetro do grafo: " << calcularDiametro() << endl;
		cout << "12. Intermediação dos vértices:\n";

		auto intermediacoes = calcularIntermediacao();
		for (size_t vertice = 0; vertice < intermediacoes.size(); ++vertice) {
		cout << "  Nó " << vertice << ": " << intermediacoes[vertice] << endl;
	}


		cout << "===================================\n";
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
    string nomeArquivo = "BHW1.dat"; 
    
    Grafo g = lerGrafoDeArquivo(nomeArquivo);
    
    g.imprimirGrafo();
    
    g.floydWarshall(); //se não fizer essa chamada, dá erro de segmentação e aí não calc nada q precisa das matrizes dist e pred
    
    g.imprimirEstatisticas();
    
    return 0;
}
