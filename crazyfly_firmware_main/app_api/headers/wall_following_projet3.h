#ifndef WALL_FOLLOWING_PROJET3_H
#define WALL_FOLLOWING_PROJET3_H
#include <stdbool.h>

/**
 * @brief Follow the wall init. Important to set the wall follower according to our need in Projet 3. 
 * Called at the begenning when the drone starts.
 *
 * @param void None
 * @return void
 */
void initFollowWallProjet3();

/**
 * @brief Prepare follow the wall every time we get back into exploration state.
 *
 * @param void None
 * @return void
 */
void prepareExploration();

/**
 * @brief End exploration orientes the mission towards its end with a landing.
 *
 * @param void None
 * @return void
 */
void endExploration();

/**
 * @brief Function to put the wall follower state into going foward. 
 * Used in return to base logic.
 *
 * @param void None
 * @return void
 */
void initstraightWallFollower();

/**
 * @brief Wall follower main function. 
 * Modified a little according to our Projet 3 needs.
 *
 * @param void None
 * @return void
 */
void followTheWall();

#endif /* WALL_FOLLOWING_PROJET3_H */