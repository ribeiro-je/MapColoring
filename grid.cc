
#include <map>
#include <utility>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <queue>
#include <vector>
#include <algorithm>
#include <cassert>
#include <stdio.h>

#include "grid.hh"

using namespace std;

typedef pair<int, int> coords;

static std::map<coords, int> grid;
static std::vector<coords> path;

// map of where detected objects are located and their color
static std::map<coords, std::string> objects;

static int
iclamp(int xmin, int xx, int xmax)
{
    if (xx < xmin)
        return xmin;
    if (xx > xmax)
        return xmax;
    return xx;
}

static float
cx(coords cc)
{
    return cc.first * CELL_SIZE;
}

static float
cy(coords cc)
{
    return cc.second * CELL_SIZE;
}

static string
coords_to_s(coords cc)
{
    char temp[100];
    float xx = cx(cc);
    float yy = cy(cc);
    snprintf(temp, 100, "(k:%.02f,%.02f)", xx, yy);
    return string(temp);
}

static int
grid_get(coords kk)
{
    try
    {
        return grid.at(kk);
    }
    catch (const std::out_of_range &_ee)
    {
        return 0;
    }
}

static void
grid_put(coords kk, int xx)
{
    //cout << "put " << coords_to_s(kk) << " " << xx << endl;
    grid[kk] = xx;
}

static void
grid_inc(coords kk, int dv)
{
    int vv = grid_get(kk);
    grid_put(kk, iclamp(0, vv + dv, 100));
}

static int
c2k(float cc)
{
    return int(round(cc / CELL_SIZE));
}

static coords
key(float xx, float yy)
{
    return make_pair(c2k(xx), c2k(yy));
}

vector<coords>
neibs(coords aa)
{
    int xx = aa.first;
    int yy = aa.second;

    vector<coords> ys;

    if (abs(xx) > 100 || abs(yy) > 100)
    {
        return ys;
    }

    ys.push_back(make_pair(xx, yy + 1));
    ys.push_back(make_pair(xx, yy - 1));
    ys.push_back(make_pair(xx + 1, yy));
    ys.push_back(make_pair(xx - 1, yy));
    ys.push_back(make_pair(xx + 1, yy + 1));
    ys.push_back(make_pair(xx + 1, yy - 1));
    ys.push_back(make_pair(xx - 1, yy + 1));
    ys.push_back(make_pair(xx - 1, yy - 1));

    return ys;
}

void grid_apply_hit(LaserHit hit, Pose pose)
{
    set<coords> cells;

    for (float ds = 0.0f; ds < (hit.range - CELL_SIZE - 0.1); ds += 0.1f)
    {
        float xx = pose.x + ds * cos(pose.t + hit.angle);
        float yy = pose.y + ds * sin(pose.t + hit.angle);
        cells.insert(key(xx, yy));
    }

    for (auto cell : cells)
    {
        grid_inc(cell, -2);
    }

    float hx = pose.x + hit.range * cos(pose.t + hit.angle);
    float hy = pose.y + hit.range * sin(pose.t + hit.angle);
    coords hk = key(hx, hy);
    grid_inc(hk, +8);

    for (auto cell : neibs(hk))
    {
        grid_inc(cell, +4);
    }

    /*
    cout << "Hit @ " << hx << "," << hy
         << " => " << coords_to_s(hk) << endl;
    */
}

int stringToInt(std::string color)
{
    if (color == "red")
    {
        return 1;
    }
    if (color == "orange")
    {
        return 2;
    }
    if (color == "brown")
    {
        return 3;
    }
    if (color == "blue")
    {
        return 4;
    }
    if (color == "yellow")
    {
        return 5;
    }
    if (color == "green")
    {
        return 6;
    }
    return 0;
}

void grid_apply_hit_color(float dist, float angle, Pose pose, std::string color)
{
    set<coords> cells;

    for (float ds = 0.0f; ds < (dist - CELL_SIZE - 0.1); ds += 0.1f)
    {
        float xx = pose.x + ds * cos(pose.t + angle);
        float yy = pose.y + ds * sin(pose.t + angle);
        cells.insert(key(xx, yy));
    }

    for (auto cell : cells)
    {
        grid_inc(cell, -2);
    }

    float hx = pose.x + dist * cos(pose.t + angle);
    float hy = pose.y + dist * sin(pose.t + angle);
    coords hk = key(hx, hy);
    grid_inc(hk, +8);
    for (auto cell : neibs(hk))
    {
        grid_inc(cell, +4);
    }
    objects.insert(std::pair(hk, color));
}

Mat grid_view(Pose pose, std::vector<coords> path)
{
    //cout << "get grid_view @ " << pose.to_s() << endl;
    //cout << fixed << setprecision(2) << endl;

    Mat gv(VIEW_SIZE, VIEW_SIZE, CV_8UC3);

    set<coords> path_cells;
    for (auto cell : path)
    {
        path_cells.insert(cell);
    }

    for (int ii = 0; ii < VIEW_SIZE; ++ii)
    {
        float dx = CELL_SIZE * (ii - (VIEW_SIZE / 2));
        float xx = pose.x + dx;

        for (int jj = 0; jj < VIEW_SIZE; ++jj)
        {
            float dy = CELL_SIZE * (jj - (VIEW_SIZE / 2));
            float yy = pose.y + dy;

            coords kk = key(xx, yy);
            int vv = 250 - (2.5 * grid_get(kk));

            // if object set correct color
            if (objects.find(kk) != objects.end())
            {
                std::string color = objects[kk];
                switch (stringToInt(color))
                {
                case 1: // red
                    gv.at<Vec3b>(jj, ii) = Vec3b(255, 0, 0);
                    break;
                case 2: // orange
                    gv.at<Vec3b>(jj, ii) = Vec3b(255, 178, 102);
                    break;
                case 3: // brown
                    gv.at<Vec3b>(jj, ii) = Vec3b(165, 42, 42);
                    break;
                case 4: // blue
                    gv.at<Vec3b>(jj, ii) = Vec3b(0, 191, 255);
                    break;
                case 5: // yellow
                    gv.at<Vec3b>(jj, ii) = Vec3b(255, 255, 0);
                    break;
                case 6: // green
                    gv.at<Vec3b>(jj, ii) = Vec3b(173, 255, 47);
                    break;
                default:
                    break;
                }
            }
            else
            {
                // Walls
                gv.at<Vec3b>(jj, ii) = Vec3b(vv, vv, vv);
            }

            if (path_cells.find(kk) != path_cells.end())
            {
                // yes path
                gv.at<Vec3b>(jj, ii) = Vec3b(0, 0, 255);
            }
        }
    }

    return gv;
}

class SearchCell
{
public:
    coords cell;
    float dist;

    SearchCell(coords cc, float dd)
        : cell(cc), dist(dd)
    {
    }
};

static float
distance(coords aa, coords bb)
{
    float dx = bb.first - aa.first;
    float dy = bb.second - aa.second;
    return sqrt(dx * dx + dy * dy);
}

class MoreDist
{
public:
    constexpr bool
    operator()(const SearchCell &aa, const SearchCell &bb)
    {
        return aa.dist > bb.dist;
    }
};

typedef priority_queue<
    SearchCell,
    vector<SearchCell>,
    MoreDist>
    search_queue;

// A* search
// ref: https://en.wikipedia.org/wiki/A*_search_algorithm
void grid_find_path(float x0, float y0, float x1, float y1)
{
    coords cell0 = key(x0, y0);
    coords cell1 = key(x1, y1);

    /*
    cout << "finding path from " << coords_to_s(cell0)
         << " to " << coords_to_s(cell1) << endl;
    */

    bool done = false;
    set<coords> seen;
    search_queue queue;
    map<coords, float> costs;
    map<coords, coords> prev;

    seen.insert(cell0);
    costs[cell0] = 0.0f;
    queue.push(SearchCell(cell0, 1.0f));

    while (!queue.empty())
    {
        SearchCell sc = queue.top();
        queue.pop();

        coords cc = sc.cell;
        //cout << "expanding " << coords_to_s(cc) << endl;

        if (cc == cell1)
        {
            done = true;
            break;
        }

        for (auto nn : neibs(cc))
        {
            if (seen.find(nn) != seen.end())
            {
                continue;
            }

            seen.insert(nn);
            prev[nn] = cc;
            costs[nn] = costs[cc] + distance(cc, nn);

            float penalty = 1.0f;

            int vv = grid_get(nn);
            if (vv > 10)
            {
                penalty += vv * (10 * 1000 * 1000);
            }

            for (auto n2 : neibs(nn))
            {
                int v2 = grid_get(n2);
                penalty += iclamp(0, 3 * v2, 30);
            }

            float score = penalty + (costs[cc] + distance(nn, cell1));
            queue.push(SearchCell(nn, score));
        }
    }

    path.clear();

    if (!done)
    {
        cout << "no path found" << endl;
        return;
    }

    coords cc = cell1;

    while (cc != cell0)
    {
        path.push_back(cc);
        cc = prev[cc];
    }

    reverse(path.begin(), path.end());

    /*
    cout << endl << "=== path ===" << endl;
    for (auto item : path) {
        cout << coords_to_s(item) << " ";
    }
    cout << endl;
    */
}

static float
ang_diff(float aa, float bb)
{
    float cc = aa - bb;
    while (cc > M_PI)
    {
        cc -= 2 * M_PI;
    }
    while (cc < -M_PI)
    {
        cc += 2 * M_PI;
    }
    return cc;
}

float grid_goal_angle(Pose pose)
{
    if (path.empty())
    {
        return 0.0f;
    }

    int ii = min(3, int(path.size() - 1));
    coords tgt = path[ii];

    float dx = cx(tgt) - pose.x;
    float dy = cy(tgt) - pose.y;
    float ga = atan2(dy, dx);

    float ra = ang_diff(ga, pose.t);

    cout << "ga = " << ga << "; "
         << "ra = " << ra << endl;

    return ra;
}
