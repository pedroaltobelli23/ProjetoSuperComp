#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>
#include <tuple>
#include <omp.h>


// Função para read o graph a partir do arquivo de entrada
std::vector<std::vector<int>> readgraph(const std::string &fileName, int &numVertices)
{
    std::ifstream ifs(fileName, std::ifstream::in);
    int numArestas;
    ifs >> numVertices >> numArestas;

    std::vector<std::vector<int>> graph(numVertices, std::vector<int>(numVertices, 0));

    for (int i = 0; i < numArestas; ++i)
    {
        int u, v;
        ifs >> u >> v;
        graph[u - 1][v - 1] = 1;
        graph[v - 1][u - 1] = 1; // O graph é não direcionado
    }

    ifs.close();

    return graph;
}

std::vector<int> findClick(const std::vector<std::vector<int>>& graph, int& numVertices, std::vector<int> candidates, std::vector<int> clique, int c, std::vector<int>& newCandidates)
{
    candidates.erase(remove(candidates.begin(), candidates.end(), c), candidates.end());

    bool adjacenteATodos = false;
    
    clique.push_back(c);

    for (int u : candidates)
    {
        adjacenteATodos = all_of(clique.begin(), clique.end(), [&](int v) {
            return graph[u][v] != 0;
        });

        if (adjacenteATodos)
        {
            newCandidates.push_back(u);
            numVertices++;
        }
    }

    return clique;
}

void findMaxClickGlobal(std::vector<std::vector<int>> graph, int numVertices,std::vector<int> candidates,std::vector<int> clique,std::vector<int>& maxClick){
    // Pega um candidato na lista de candidatos
    for(auto& c:candidates){
        std::vector<int> newCandidates;
        int verticesnewCandidates=0;
        std::vector<int> nowClick;

        //Retorna o clique de agora e novos candidatos
        nowClick = findClick(graph,verticesnewCandidates,candidates,clique,c,newCandidates);
        
        int nowClickSize = nowClick.size();
        int maxclickSize = maxClick.size();

        if(verticesnewCandidates>0){
            // junta nowClick e newCandidates. se esse novo vetor esta em maxClick, para de rodar
            // Criando um novo vetor e reservando espaço para a junção

            std::vector<int> vetorResultado;
            vetorResultado.reserve(nowClick.size() + newCandidates.size());

            // Copiando os elementos de vetor1 para vetorResultado
            vetorResultado.insert(vetorResultado.end(), nowClick.begin(), nowClick.end());

            // Copiando os elementos de vetor2 para vetorResultado
            vetorResultado.insert(vetorResultado.end(), newCandidates.begin(), newCandidates.end());

            // Criando cópias ordenadas dos vetores originais
            std::vector<int> vetor1Ordenado = vetorResultado;
            std::vector<int> vetor2Ordenado = maxClick;

            // Ordenando as cópias
            std::sort(vetor1Ordenado.begin(), vetor1Ordenado.end());
            std::sort(vetor2Ordenado.begin(), vetor2Ordenado.end());

            // Verificando se vetor2 está contido em vetor1
            bool contido = std::includes(vetor1Ordenado.begin(), vetor1Ordenado.end(), vetor2Ordenado.begin(), vetor2Ordenado.end());


            if(maxclickSize==0 || nowClickSize+verticesnewCandidates>maxclickSize && !contido){
                findMaxClickGlobal(graph,numVertices,newCandidates,nowClick,maxClick);
            }
        }else if(nowClick.size()>=maxClick.size()){
            maxClick = nowClick;
        }

        // std::cout<<"max click ";
    
        // for(auto& a:maxClick){
        //     std::cout<<a<<" ";
        // }
        
        // std::cout<<std::endl;
    }

}

int main()
{
    double start; 
    double end; 
    start = omp_get_wtime(); 
    int numVertices;
    std::vector<std::vector<int>> graph;

    std::string fileName = "grafo.txt";

    graph = readgraph(fileName, numVertices);

    std::vector<int> candidates(numVertices);
    for (int i = 0; i < numVertices; ++i)
    {
        candidates[i] = i;
    }

    std::vector<int> maxClick;
    std::vector<int> initClick;
    findMaxClickGlobal(graph,numVertices,candidates,initClick,maxClick);
    std::cout<<"Res"<<std::endl;

    for (auto& v : maxClick) {
        std::cout<<v+1<<" ";
    }

    std::cout << std::endl;
    end = omp_get_wtime(); 
    printf("Work took %f seconds for numvertices=%d\n", end - start,numVertices);
    std::cout << "Codigo Serial " << std::endl;

    return 0;
}
