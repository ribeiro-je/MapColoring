
#include <iostream>
#include <thread>
#include <algorithm>
#include <math.h>

#include "robot.hh"
#include "grid.hh"
#include "viz.hh"

using namespace std;

void
callback(Robot* robot)
{
    if (robot->ranges.size() < 5) {
        return;
    }

    Pose pose(robot->pos_x, robot->pos_y, robot->pos_t);

    for (auto hit : robot->ranges) {
        if (hit.range < 100) {
            grid_apply_hit(hit, pose);
        }
    }

    // Goal is at x = 20, y = 0
    grid_find_path(pose.x, pose.y, 20.0f, 0.0f);

    Mat view = grid_view(pose);
    viz_show(view);

    float ang = grid_goal_angle(pose);
    float trn = clamp(-1.0, 3*ang, 1.0);

    float fwd = clamp(0.0, robot->ranges[3].range, 2.0);
    float lft = clamp(0.0, robot->ranges[2].range, 2.0);
    float rgt = clamp(0.0, robot->ranges[4].range, 2.0);

    if (abs(ang) > 0.5) {
        robot->set_vel(-trn, +trn);
        return;
    }

    if (fwd > 1.0) {
        robot->set_vel(2.0f - trn, 2.0f + trn);
        return;
    }

    // Wall follow.
    float spd = clamp(0, fwd - 1.0, 1);
    if (lft < rgt) {
        trn = 1;
        if (lft < 0.75) {
            spd = 0;
        }
    }
    else {
        if (rgt < 0.75) {
            spd = 0;
        }
        trn = -1;
    }

    //cout << "spd,trn = " << spd << "," << trn << endl;
    robot->set_vel(spd - trn, spd + trn);
}

void
robot_thread(Robot* robot)
{
    robot->do_stuff();
}

int
main(int argc, char* argv[])
{
    cout << "making robot" << endl;
    Robot robot(argc, argv, callback);

    std::thread rthr(robot_thread, &robot);

    return viz_run(argc, argv);
}
