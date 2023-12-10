#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>
#include <tuple>
#include <omp.h>
#include <mpi.h>

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

std::vector<int> findClick(const std::vector<std::vector<int>> &graph, int &numVertices, std::vector<int> candidates, std::vector<int> clique, int c, std::vector<int> &newCandidates)
{
    candidates.erase(remove(candidates.begin(), candidates.end(), c), candidates.end());

    bool adjacenteATodos = false;

    clique.push_back(c);

    for (int u : candidates)
    {
        adjacenteATodos = all_of(clique.begin(), clique.end(), [&](int v)
                                 { return graph[u][v] != 0; });

        if (adjacenteATodos)
        {
            newCandidates.push_back(u);
            numVertices++;
        }
    }

    return clique;
}

void findMaxClickGlobal(std::vector<std::vector<int>> graph, int numVertices, std::vector<int> candidates, std::vector<int> clique, std::vector<int> &maxClick,std::vector<int> smallcandidates)
{
    // Pega um candidato na lista de candidatos
    for (auto &c : smallcandidates)
    {
        std::vector<int> newCandidates;
        int verticesnewCandidates = 0;
        std::vector<int> nowClick;

        // Retorna o clique de agora e novos candidatos
        nowClick = findClick(graph, verticesnewCandidates, candidates, clique, c, newCandidates);

        int nowClickSize = nowClick.size();
        int maxclickSize = maxClick.size();

        if (verticesnewCandidates > 0)
        {
            if (maxclickSize == 0)
            {
                findMaxClickGlobal(graph, numVertices, newCandidates, nowClick, maxClick,newCandidates);
            }
            else if (nowClickSize + verticesnewCandidates > maxclickSize)
            {
                findMaxClickGlobal(graph, numVertices, newCandidates, nowClick, maxClick,newCandidates);
            }
        }
        else if (nowClick.size() >= maxClick.size())
        {
            maxClick = nowClick;
        }
    }
}

int main(int argc, char *argv[])
{
    int size, rank;
    double startTime, endTime; 

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // std::cout<<"Numero de processos com o 0: "<<size<<std::endl;
    int numVertices;
    std::vector<std::vector<int>> graph;
    std::string fileName = "grafo.txt";
    std::vector<int> maxClick;
    std::vector<int> initClick;

    graph = readgraph(fileName, numVertices);
    // std::cout<<numVertices<<std::endl;

    int sizeVec = numVertices/(size-1); //Tamanho de cada slice de vector
    int atualsizeofMaxfromRank;
    int sizeofMaxfromRank;
    int rankMax;
    // std::cout<<sizeVec<<std::endl;
    // std::cout<<sizeVec % (size-1)<<std::endl;
    
    std::vector<int> recv_full_array(numVertices);
    std::vector<int> recv_array(sizeVec);

    if(rank==0){
        std::vector<int> fullcandidates(numVertices);

        for (int i = 0; i < numVertices; i++)
        {
            fullcandidates[i] = i;
            // printf("%d",fullcandidates[i]);
        }

        // std::cout<<std::endl;

        //Sending full array to all
        for(int i=1;i<size;i++){
            // std::cout<<"Mandou "<<i<<std::endl;
            MPI_Send(fullcandidates.data(),numVertices,MPI_INT,i,0,MPI_COMM_WORLD);
        }

        //Sending small array to all
        for(int i=1;i<size;i++){
            // std::cout<<"Mandou "<<i<<std::endl;
            MPI_Send(fullcandidates.data() + (i-1)*sizeVec,sizeVec,MPI_INT,i,0,MPI_COMM_WORLD);
        }

        startTime = MPI_Wtime();
        //Recebe o tamanho de cada processo
        for(int i=1;i<size;i++){
            MPI_Recv(&sizeofMaxfromRank,1,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            if (sizeofMaxfromRank>=atualsizeofMaxfromRank){
                atualsizeofMaxfromRank=sizeofMaxfromRank;
                rankMax = i;
            }
        }

        //Sending small array to all
        for(int i=1;i<size;i++){
            // std::cout<<"Mandou "<<i<<std::endl;
            MPI_Send(&rankMax,1,MPI_INT,i,0,MPI_COMM_WORLD);
        }

        // std::cout<<"FOI MASTER"<<std::endl;
    }else{
        MPI_Recv(recv_full_array.data(),numVertices,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);        
        MPI_Recv(recv_array.data(),sizeVec,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        
        std::vector<int> maxClick;
        std::vector<int> initClick;
        findMaxClickGlobal(graph, numVertices, recv_full_array, initClick, maxClick,recv_array);

        // std::cout << "Res do processo:" <<rank<< std::endl;
        
        // std::cout<<std::endl;
        int sizeMaxClick = maxClick.size();
        //Envia o tamanho para o rank0
        MPI_Send(&sizeMaxClick,1,MPI_INT,0,0,MPI_COMM_WORLD);

        MPI_Recv(&rankMax,1,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        if(rank==rankMax){
            printf("Resposta:  \n",rank);
            for (auto &v : maxClick)
            {
                std::cout << v + 1 << " ";
            }
        }
    }

    endTime = MPI_Wtime();  // Registra o tempo de término

    if (rank == 0) {
        // Apenas o processo de rank 0 exibirá o tempo total
        std::cout << "Tempo de execução: " << endTime - startTime << " segundos." << std::endl;
        std::cout << "Tamanho do grafo: " << numVertices << std::endl;
        std::cout << "Codigo MPI " << std::endl;
    }

    MPI_Finalize();
    return 0;
}

