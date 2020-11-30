#include <iostream>
#include <algorithm>

#include "Grid.hh"

// https://weblog.jamisbuck.org/2015/1/15/better-recursive-division-algorithm.html

using namespace std;

Grid::Grid(int sz)
    : size(sz)
{
    for (int ii = 0; ii < size; ++ii) {
        for (int jj = 0; jj < size; ++jj) {
            this->cells[key(ii,jj)] = 0;
        }
    }
}

vector<key>
Grid::keys()
{
    vector<key> ys;
    for (auto it = cells.begin(); it != cells.end(); ++it) {
        ys.push_back(it->first);
    }
    return ys;
}

void
Grid::split()
{
    vector<key> xs = keys();
    random_shuffle(xs.begin(), xs.end());


}

void
Grid::dump()
{
    for (int ii = 0; ii < size; ++ii) {
        for (int jj = 0; jj < size; ++jj) {
            int cell = this->cells[key(ii,jj)];
            if (cell == 0) {
                cout << " ";
            }
            else {
                cout << cell;
            }
        }
        cout << endl;
    }
}
