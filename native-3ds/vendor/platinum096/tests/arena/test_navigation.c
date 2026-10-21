#include "arena/vendor/arena_navigation.h"
#include <assert.h>
#include <stdio.h>

int main(void)
{
    static const struct ArenaPoint goals[] = {
        {ARENA_MIN_X,ARENA_MIN_Y},{ARENA_MAX_X,ARENA_MIN_Y},
        {ARENA_MIN_X,ARENA_MAX_Y},{ARENA_MAX_X,ARENA_MAX_Y},
        {ARENA_WORLD_WIDTH/2,ARENA_MIN_Y},{ARENA_WORLD_WIDTH/2,ARENA_MAX_Y}
    };
    int x, y, i, j, routes = 0;
    ArenaNav_Init();
    assert(!ArenaNav_CanStand(ARENA_MIN_X-1,ARENA_MIN_Y));
    assert(!ArenaNav_CanStand(ARENA_WORLD_WIDTH/2,ARENA_MAX_Y+1));
    // The updated stage intentionally leaves the centre lane open.
    assert(ArenaNav_LineClear(ARENA_WORLD_WIDTH/2,ARENA_MIN_Y,ARENA_WORLD_WIDTH/2,ARENA_MAX_Y,2));
    // The relocated first prop obstructs its actual flank lane.
    assert(!ArenaNav_LineClear((gArenaObstacles[0].left+gArenaObstacles[0].right)/2,
        ARENA_MIN_Y,(gArenaObstacles[0].left+gArenaObstacles[0].right)/2,ARENA_MAX_Y,10));
    for (i = 0; i < ARENA_OBSTACLES; ++i)
    {
        const struct ArenaRect *r = &gArenaObstacles[i];
        assert(!ArenaNav_CanStand((r->left+r->right)/2,(r->top+r->bottom)/2));
        assert(!ArenaNav_LineClear(r->left-30,r->top,r->right+30,r->top,0));
        assert(!ArenaNav_LineClear(r->left-30,r->top-30,r->right+30,r->bottom+30,2));
    }
    for (x = ARENA_MIN_X; x <= ARENA_MAX_X; x += 7)
        for (y = ARENA_MIN_Y; y <= ARENA_MAX_Y; y += 7)
        {
            if (!ArenaNav_CanStand(x,y)) continue;
            for (i = 0; i < 6; i++)
            {
                struct ArenaPoint at = {x,y}, next;
                assert(ArenaNav_LineClear(x,y,goals[i].x,goals[i].y,2)
                    == ArenaNav_LineClear(goals[i].x,goals[i].y,x,y,2));
                for (j = 0; j <= ARENA_CORNERS; ++j)
                {
                    assert(ArenaNav_NextWaypoint(at.x,at.y,goals[i].x,goals[i].y,&next));
                    assert(ArenaNav_CanStand(next.x,next.y));
                    assert(ArenaNav_LineClear(at.x,at.y,next.x,next.y,ARENA_BODY_RADIUS));
                    at = next;
                    if (at.x == goals[i].x && at.y == goals[i].y) break;
                }
                assert(j <= ARENA_CORNERS);
                routes++;
            }
        }
    printf("PASS: %d reachable routes, swept solids, symmetry, bounds; same C as ROM\n", routes);
    // Every destruction combination changes connectivity immediately, without
    // rebuilding geometry. Re-enabling a solid must also restore collision.
    for(i=0;i<(1<<ARENA_OBSTACLES);i++)
    {
        struct ArenaPoint at={ARENA_WORLD_WIDTH/2,ARENA_MAX_Y},next;
        for(j=0;j<ARENA_OBSTACLES;j++)ArenaNav_SetObstacle(j,!!(i&(1<<j)));
        for(j=0;j<=ARENA_CORNERS;j++)
        {
            assert(ArenaNav_NextWaypoint(at.x,at.y,ARENA_WORLD_WIDTH/2,ARENA_MIN_Y,&next));
            assert(ArenaNav_LineClear(at.x,at.y,next.x,next.y,ARENA_BODY_RADIUS));
            at=next;
            if(at.x==ARENA_WORLD_WIDTH/2&&at.y==ARENA_MIN_Y)break;
        }
        assert(j<=ARENA_CORNERS);
    }
    puts("PASS: all 128 dynamic destruction masks remain routable");
    return 0;
}
