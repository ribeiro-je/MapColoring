
#include "Grid.hh"

int
main(int _ac, char* _av[])
{
    Grid gg(10);
    gg.split();
    gg.dump();
    return 0;
}
