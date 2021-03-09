
#include <sys/time.h>

#include "../include/qureg.hpp"

int main(int argc, char **argv)
{
    qhipster::mpi::Environment env(argc, argv);
    if (env.IsUsefulRank() == false)
        return 0;
    int myrank = env.GetStateRank();
    if (myrank == 0)
        std::cout << " ---------------------- \n"
                  << " ---- Hello world! ---- \n"
                  << " ---------------------- \n";

    hello();
}
