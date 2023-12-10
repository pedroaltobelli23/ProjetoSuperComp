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

    #pragma omp parallel for private(adjacenteATodos)
    for (int i = 0; i < candidates.size(); ++i)
    {
        int u = candidates[i];
        adjacenteATodos = true;

        #pragma omp parallel for reduction(&&:adjacenteATodos)
        for (int j = 0; j < clique.size(); ++j)
        {
            int v = clique[j];
            if (graph[u][v] == 0)
            {
                adjacenteATodos = false;
            }
        }

        if (adjacenteATodos)
        {
            #pragma omp critical
            {
                newCandidates.push_back(u);
                numVertices++;
            }
        }
    }

    return clique;
}

void findMaxClickGlobal(std::vector<std::vector<int>> graph, int numVertices,std::vector<int> candidates,std::vector<int> clique,std::vector<int>& maxClick){
    // Pega um candidato na lista de candidatos
    #pragma omp parallel for
    for(auto& c:candidates){
        std::vector<int> newCandidates;
        int verticesnewCandidates=0;
        std::vector<int> nowClick;

        //Retorna o clique de agora e novos candidatos
        nowClick = findClick(graph,verticesnewCandidates,candidates,clique,c,newCandidates);
        
        int nowClickSize = nowClick.size();
        int maxclickSize = maxClick.size();

        if(verticesnewCandidates>0){
            if(maxclickSize==0){
                findMaxClickGlobal(graph,numVertices,newCandidates,nowClick,maxClick);
            }else if(nowClickSize+verticesnewCandidates>maxclickSize){
                findMaxClickGlobal(graph,numVertices,newCandidates,nowClick,maxClick);
            }
        }else if(nowClick.size()>=maxClick.size()){
            #pragma omp critical
            {
                maxClick = nowClick;
            }
        }
        
    }

}

int main(int argc, char *argv[])
{
    double start; 
    double end; 
    start = omp_get_wtime(); 
    int numVertices;
    std::vector<std::vector<int>> graph;

    std::string fileName = argv[1];

    graph = readgraph(fileName, numVertices);

    std::vector<int> candidates(numVertices);
    for (int i = 0; i < numVertices; ++i)
    {
        candidates[i] = i;
    }

    std::vector<int> maxClick;
    std::vector<int> initClick;
    findMaxClickGlobal(graph,numVertices,candidates,initClick,maxClick);

    for (auto& v : maxClick) {
        std::cout<<v+1<<" ";
    }

    std::cout << std::endl;
    end = omp_get_wtime(); 
    printf("Work took %f seconds for numvertices=%d\n", end - start,numVertices);
    std::cout << "Codigo OPENMP " << std::endl;

    return 0;
}