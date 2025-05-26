#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <sstream>
#include <limits>
#include <iomanip>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace chrono;

class Grafo
{
public:
    enum TipoServico { NODO, ARESTA, ARCO };

    struct Servico
    {
        int id;
        TipoServico tipo;
        int origem, destino;
        int demanda, custoServico;
        bool atendido = false;
    };

    int V, E, A, ReqN, ReqE, ReqA;
    int numVeiculos, capacidade, deposito;
    vector<list<pair<int, int>>> adj;
    vector<vector<int>> matrizAdj;
    vector<vector<int>> matrizDistancia;
    vector<vector<int>> matrizPredecessor;
    vector<bool> verticesRequeridos;
    vector<Servico> servicosObrigatorios;

    Grafo(int V) : V(V), verticesRequeridos(V + 1, false)
    {
        adj.resize(V + 1);
        matrizAdj.assign(V + 1, vector<int>(V + 1, numeric_limits<int>::max()));
        for (int i = 1; i <= V; i++) matrizAdj[i][i] = 0;
    }

    void adicionarAresta(int origem, int destino, int peso, bool requerida, int demanda, int custoServico)
    {
        (void)requerida;
        (void)demanda;
        (void)custoServico;

        adj[origem].push_back({destino, peso});
        adj[destino].push_back({origem, peso});
        matrizAdj[origem][destino] = peso;
        matrizAdj[destino][origem] = peso;
    }

    void adicionarArco(int origem, int destino, int peso, bool requerido, int demanda, int custoServico)
    {
        (void)requerido;
        (void)demanda;
        (void)custoServico;

        adj[origem].push_back({destino, peso});
        matrizAdj[origem][destino] = peso;
    }

    void marcarVerticeRequerido(int vertice)
    {
        verticesRequeridos[vertice] = true;
    }

    void floydWarshall()
    {
        matrizDistancia = matrizAdj;
        matrizPredecessor.assign(V + 1, vector<int>(V + 1, -1));
        for (int i = 1; i <= V; ++i)
            for (int j = 1; j <= V; ++j)
                if (matrizAdj[i][j] < numeric_limits<int>::max())
                    matrizPredecessor[i][j] = i;

        for (int k = 1; k <= V; ++k)
            for (int i = 1; i <= V; ++i)
                for (int j = 1; j <= V; ++j)
                    if (matrizDistancia[i][k] < numeric_limits<int>::max() &&
                        matrizDistancia[k][j] < numeric_limits<int>::max() &&
                        matrizDistancia[i][j] > matrizDistancia[i][k] + matrizDistancia[k][j])
                    {
                        matrizDistancia[i][j] = matrizDistancia[i][k] + matrizDistancia[k][j];
                        matrizPredecessor[i][j] = matrizPredecessor[k][j];
                    }
    }
};

struct Visita {
    char tipo;
    int idServico;
    int origem, destino;
};

struct Rota {
    vector<Visita> visitas;
    int demandaTotal = 0;
    int custoTotal = 0;
};

vector<Rota> construirComPathScanning(Grafo &g)
{
    vector<Rota> rotas;
    int capacidadeVeiculo = g.capacidade;
    int deposito = g.deposito;

    while (true)
    {
        bool existeServico = false;
        for (const auto &s : g.servicosObrigatorios)
            if (!s.atendido) { existeServico = true; break; }
        if (!existeServico) break;

        Rota rota;
        rota.visitas.push_back({'D', 0, deposito, deposito});
        int cargaAtual = 0, custoAtual = 0, posicaoAtual = deposito;

        while (true)
        {
            int melhorIndice = -1;
            int melhorScore = numeric_limits<int>::max();

            for (int i = 0; i < (int)g.servicosObrigatorios.size(); ++i)
            {
                auto &s = g.servicosObrigatorios[i];
                if (s.atendido || s.demanda + cargaAtual > capacidadeVeiculo) continue;

                int dist = g.matrizDistancia[posicaoAtual][s.origem];
                int score = dist; // Critério principal: menor distância ao serviço

                if (score < melhorScore)
                {
                    melhorScore = score;
                    melhorIndice = i;
                }
            }

            if (melhorIndice == -1) break;

            auto &s = g.servicosObrigatorios[melhorIndice];
            custoAtual += g.matrizDistancia[posicaoAtual][s.origem] + s.custoServico;
            cargaAtual += s.demanda;
            s.atendido = true;

            rota.visitas.push_back({'S', s.id, s.origem, s.destino});
            posicaoAtual = s.destino;
        }

        int retorno = g.matrizDistancia[posicaoAtual][deposito];
        if (retorno != numeric_limits<int>::max()) custoAtual += retorno;

        rota.visitas.push_back({'D', 0, deposito, deposito});
        rota.demandaTotal = cargaAtual;
        rota.custoTotal = custoAtual;
        rotas.push_back(rota);
    }

    return rotas;
}

Grafo lerGrafoDeArquivo(const string &nomeArquivo)
{
    ifstream arquivo(nomeArquivo);
    if (!arquivo) { cerr << "Erro ao abrir o arquivo!\n"; exit(1); }

    string linha;
    int V = 0, E = 0, A = 0, ReqN = 0, ReqE = 0, ReqA = 0;
    int numVeiculos = 0, capacidade = 0, deposito = 0;

    while (getline(arquivo, linha))
    {
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
    g.V = V; g.E = E; g.A = A; g.ReqN = ReqN; g.ReqE = ReqE; g.ReqA = ReqA;
    g.numVeiculos = numVeiculos; g.capacidade = capacidade; g.deposito = deposito;

    int idServico = 1;
    arquivo.clear(); arquivo.seekg(0);

    while (getline(arquivo, linha))
    {
        if (linha.find("ReN.") != string::npos)
        {
            while (getline(arquivo, linha) && !linha.empty() && linha[0] == 'N')
            {
                istringstream iss(linha); string label; int node, demand, scost;
                iss >> label >> node >> demand >> scost;
                g.marcarVerticeRequerido(node);
                g.servicosObrigatorios.push_back({idServico++, Grafo::NODO, node, node, demand, scost});
            }
        }
        else if (linha.find("ReE.") != string::npos)
        {
            while (getline(arquivo, linha) && !linha.empty() && linha[0] == 'E')
            {
                istringstream iss(linha); string label; int from, to, tcost, demand, scost;
                iss >> label >> from >> to >> tcost >> demand >> scost;
                g.adicionarAresta(from, to, tcost, true, demand, scost);
                g.servicosObrigatorios.push_back({idServico++, Grafo::ARESTA, from, to, demand, scost});
            }
        }
        else if (linha.find("EDGE") != string::npos)
        {
            while (getline(arquivo, linha) && !linha.empty() && linha[0] == 'N')
            {
                istringstream iss(linha); string label; int from, to, tcost;
                iss >> label >> from >> to >> tcost;
                g.adicionarAresta(from, to, tcost, false, 0, 0);
            }
        }
        else if (linha.find("ReA.") != string::npos)
        {
            while (getline(arquivo, linha) && !linha.empty() && linha[0] == 'A')
            {
                istringstream iss(linha); string label; int from, to, tcost, demand, scost;
                iss >> label >> from >> to >> tcost >> demand >> scost;
                g.adicionarArco(from, to, tcost, true, demand, scost);
                g.servicosObrigatorios.push_back({idServico++, Grafo::ARCO, from, to, demand, scost});
            }
        }
        else if (linha.find("ARC") != string::npos)
        {
            while (getline(arquivo, linha) && !linha.empty() && linha[0] == 'A')
            {
                istringstream iss(linha); string label; int from, to, tcost;
                iss >> label >> from >> to >> tcost;
                g.adicionarArco(from, to, tcost, false, 0, 0);
            }
        }
    }

    arquivo.close();
    return g;
}

int main()
{
    string nomeArquivo = "BHW1.dat";
    Grafo g = lerGrafoDeArquivo(nomeArquivo);

    auto inicioReferencia = high_resolution_clock::now();
    g.floydWarshall();
    auto fimReferencia = high_resolution_clock::now();
    long long clocksReferencia = duration_cast<microseconds>(fimReferencia - inicioReferencia).count();

    auto inicioSolucao = high_resolution_clock::now();
    vector<Rota> rotas = construirComPathScanning(g);
    auto fimSolucao = high_resolution_clock::now();
    long long clocksSolucao = duration_cast<microseconds>(fimSolucao - inicioSolucao).count();

    int custoTotal = 0;
    for (const auto &rota : rotas) custoTotal += rota.custoTotal;

    ofstream saida("sol-" + nomeArquivo);
    saida << custoTotal << endl;
    saida << rotas.size() << endl;
    saida << clocksReferencia << endl;
    saida << clocksSolucao << endl;

    int idx = 1;
    for (const auto &rota : rotas)
    {
        saida << "0 1 " << idx++ << " " << rota.demandaTotal << " " << rota.custoTotal << " " << rota.visitas.size();
        for (const auto &v : rota.visitas)
        {
            if (v.tipo == 'D')
                saida << " (D " << g.deposito << "," << g.deposito << "," << g.deposito << ")";
            else
                saida << " (S " << v.idServico << "," << v.origem << "," << v.destino << ")";
        }
        saida << endl;
    }

    return 0;
}