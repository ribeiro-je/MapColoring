#ifndef GRID_HH
#define GRID_HH

#include <map>
#include <vector>
#include <utility>

using namespace std;

typedef pair<int,int> key;

class Grid {
  public:
    int size;
    map<key,int> cells;

    Grid(int sz);
    vector<key> keys();
    void split();
    void dump();
};

#endif
