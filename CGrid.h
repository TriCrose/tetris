#ifndef CGRID_H
#define CGRID_H

#include <glm/glm.hpp>
#include <vector>
#include "StringFunctions.h"

#define GRID_WIDTH 8
#define GRID_HEIGHT 12
#define GRID_DEPTH 8

#define ITERATE_THROUGH_BLOCKS for (int i = 0; i < GRID_WIDTH; i++) for (int j = 0; j < GRID_HEIGHT; j++) for (int k = 0; k < GRID_DEPTH; k++)
#define ITERATE_THROUGH_BLOCKS_BACKWARDS for (int i = GRID_WIDTH - 1; i >= 0; i--) for (int j = GRID_HEIGHT - 1; j >= 0; j--) for (int k = GRID_DEPTH - 1; k >= 0; k--)

enum COLOUR {
	INVISIBLE = 0, CYAN, BLUE, ORANGE, YELLOW, GREEN, MAGENTA, RED, WHITE
};

enum ANIMATION_TYPE {
    ANIMATION_NONE = 0,
    ANIMATION_LINE,
    ANIMATION_PLANE
};

struct ANIMATION {
    ANIMATION_TYPE type;
    unsigned int counter;
    unsigned int duration;
};

#define LINE_ANIM_DURATION 300
#define PLANE_ANIM_DURATION 400

class BLOCK {
    friend class CGrid;
private:
    COLOUR col;
    bool moving;
    int group;
    glm::vec3 rot;
    void Clear() { col = INVISIBLE; moving = false; group = 0; rot = glm::vec3(0.0f); }
    void Set(COLOUR col, bool moving, int group, glm::vec3 rot) { this->col = col; this->moving = moving; this->group = group; this->rot = rot; }
};

struct LINE {
    int height, index;
};

// Grids have to have their origin at the origin of the block positioning, and the block grid must be 8 * 12 * 8 units in size
class CGrid {
private:
    const static int BLOCK_FALL_TIME = 800;

	BLOCK m_Blocks[GRID_WIDTH][GRID_HEIGHT][GRID_DEPTH];
	int m_iScore;
	unsigned int m_uiCounter;
	unsigned int m_uiPrevCounter;
	unsigned int m_uiMarkerCounter;
	unsigned long m_ulTimer;
	bool m_bFallFaster;
	std::vector<LINE> m_FullLines;
	std::vector<int> m_FullPlanes;
	ANIMATION m_Animation;
	COLOUR m_NextShape;
	bool m_bGameOver;

	bool SpawnNewBlock(COLOUR col);
	bool DescendBlocks();
	bool IsClear();
	bool IsLineFull(int height, int index);
	bool IsPlaneFull(int height);
	std::vector<LINE> CheckForFullLines();
	std::vector<int> CheckForFullPlanes();
public:
    CGrid();
    int m_iSelectedAxis;
    bool GetInvis(int i, int j, int k) { return m_Blocks[i][j][k].col == INVISIBLE; }
    bool GetMoving(int i, int j, int k) { return m_Blocks[i][j][k].moving; }
    glm::vec3 GetColour(int i, int j, int k);
    COLOUR GetNextShape() { return m_NextShape; }
    unsigned int GetMarkerCounter() { return m_uiMarkerCounter; }
    ANIMATION GetCurrentAnimation() { return m_Animation; }
    std::vector<int> GetFullPlanes() { return m_FullPlanes; }
    int GetScore() { return m_iScore; }
    bool GetGameOver() { return m_bGameOver; }
    std::string GetTimerAsString();
    void Reset();

    void MakeAllStationary();
    void Tick(int ms);
    bool MoveBlocksHorizontal(int Dir);
    bool MoveBlocksVertical(int Dir);
    bool RotateBlocks(int Axis, float Degrees);     // Axis is 0 for x, 1 for y, 2 for z

    void DropBlock();
    void SetFallFaster(bool FallFaster);
};

#endif // CGRID_H
