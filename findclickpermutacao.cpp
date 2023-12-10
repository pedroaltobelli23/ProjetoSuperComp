#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>
using namespace std;

// Função para ler o grafo a partir do arquivo de entrada
vector<vector<int>> LerGrafo(const string &nomeArquivo, int &numVertices)
{
    ifstream ifs(nomeArquivo, ifstream::in);
    int numArestas;
    ifs >> numVertices >> numArestas;

    vector<vector<int>> grafo(numVertices, vector<int>(numVertices, 0));

    for (int i = 0; i < numArestas; ++i)
    {
        int u, v;
        ifs >> u >> v;
        grafo[u - 1][v - 1] = 1;
        grafo[v - 1][u - 1] = 1; // O grafo é não direcionado
    }

    ifs.close();

    return grafo;
}

vector<int> EncontrarCliqueMaxima(vector<vector<int>> grafo, int numVertices,vector<int> candidatos)
{
    vector<int> cliqueMaxima; // Comeca vazia
    vector<int> listaNos;     // Comeca vazia

    bool podeAdicionar = true;
    bool adjacenteATodos = false;

    while (candidatos.size() > 0)
    {
        int v = candidatos.back();
        candidatos.pop_back();
        podeAdicionar = true;
        int sizeCliqueMaxima = cliqueMaxima.size();

        for (int index_u = 0; index_u < sizeCliqueMaxima; index_u++)
        {
            int u = cliqueMaxima[index_u];
            if (grafo[u][v] == 0)
            {
                podeAdicionar = false;
                break;
            }
        }

        if (podeAdicionar)
        {
            cliqueMaxima.push_back(v);
            vector<int> novosCandidatos;
            int sizeCandidatos = candidatos.size();
            for (int index_u = 0; index_u < sizeCandidatos; index_u++)
            {
                int u = candidatos[index_u];
                adjacenteATodos = true;
                int sizeCliqueMaxima2 = cliqueMaxima.size();
                for (int index_c = 0; index_c < sizeCliqueMaxima2; index_c++)
                {
                    int c = cliqueMaxima[index_c];
                    if (grafo[u][c] == 0)
                    {
                        adjacenteATodos = false;
                        break;
                    }
                }

                if (adjacenteATodos)
                {
                    novosCandidatos.push_back(u);
                }
            }

            candidatos = novosCandidatos;
        }
    }

    return cliqueMaxima;
}

vector<vector<int>> generateCombinations(vector<int> candidatos, int numVertices) {
    vector<vector<int>> result;

    do {
        result.push_back(candidatos);
    } while (next_permutation(candidatos.begin(), candidatos.end()));

    return result;
}

bool cliqueExiste(const vector<int>& clique, const vector<vector<int>>& allcliques) {
    for (const auto& existingClique : allcliques) {
        if (existingClique == clique) {
            return true; // O clique já existe
        }
    }
    return false; // O clique não existe
}

int main()
{
    int numVertices;
    vector<vector<int>> grafo;

    string nomeArquivo = "grafo.txt";

    grafo = LerGrafo(nomeArquivo, numVertices);

    vector<int> candidatos(numVertices);
    for (int i = 0; i < numVertices; ++i)
    {
        candidatos[i] = i;
    }

    vector<vector<int>> combinations = generateCombinations(candidatos, numVertices);

    vector<vector<int>> allcliques;
    // Verificar cada combinação
    for (const auto& combination : combinations)
    {
        vector<int> cliqueMaxima = EncontrarCliqueMaxima(grafo, numVertices, combination);
        sort(cliqueMaxima.begin(),cliqueMaxima.end());
        if (!cliqueExiste(cliqueMaxima, allcliques)) {
            allcliques.push_back(cliqueMaxima);
        }
    }

    for (const auto& clique : allcliques) {
        for (int vertex : clique) {
            cout << vertex+1 << " ";
        }
        cout << endl;
    }

    return 0;
}
