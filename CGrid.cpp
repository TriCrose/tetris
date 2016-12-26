#define GLM_FORCE_INLINE
#define GLM_FORCE_RADIANS
#include <glm/gtx/rotate_vector.hpp>

#include "CGrid.h"
#include "CWindowManager.h"

CGrid::CGrid() {
    Reset();
}

glm::vec3 CGrid::GetColour(int i, int j, int k) {
    switch (m_Blocks[i][j][k].col) {
    case CYAN:
        return glm::vec3(0.0f, 1.0f, 1.0f);
    case BLUE:
        return glm::vec3(0.0f, 0.0f, 1.0f);
    case ORANGE:
        return glm::vec3(1.0f, 0.57f, 0.0f);
    case YELLOW:
        return glm::vec3(1.0f, 1.0f, 0.0f);
    case GREEN:
        return glm::vec3(0.0f, 1.0f, 0.0f);
    case MAGENTA:
        return glm::vec3(1.0f, 0.0f, 1.0f);
    case RED:
        return glm::vec3(1.0f, 0.0f, 0.0f);
    case WHITE:
        return glm::vec3(1.0f, 1.0f, 1.0f);
    default:
        return glm::vec3(0.0f);
    }
}

bool CGrid::IsLineFull(int height, int index) {
	if (index > 0) {	// Z-axis line
		for (int k = 0; k < GRID_DEPTH; k++) if (m_Blocks[index - 1][height][k].col == INVISIBLE || m_Blocks[index - 1][height][k].moving) return false;
	} else {			// X-axis line
		for (int i = 0; i < GRID_WIDTH; i++) if (m_Blocks[i][height][-(index) - 1].col == INVISIBLE || m_Blocks[i][height][-(index) - 1].moving) return false;
	}

	bool AllWhite = true;
	if (index > 0) {	// Z-axis line
		for (int k = 0; k < GRID_DEPTH; k++) if (m_Blocks[index - 1][height][k].col != WHITE) AllWhite = false;
	} else {			// X-axis line
		for (int i = 0; i < GRID_WIDTH; i++) if (m_Blocks[i][height][-(index) - 1].col != WHITE) AllWhite = false;
	}

	if (AllWhite) return false;
	return true;
}

bool CGrid::IsPlaneFull(int height) {
	for (int i = 0; i < GRID_WIDTH; i++) for (int k = 0; k < GRID_DEPTH; k++) if (m_Blocks[i][height][k].col == INVISIBLE || m_Blocks[i][height][k].moving) return false;
	return true;
}

std::vector<LINE> CGrid::CheckForFullLines() {
    std::vector<LINE> lines;
    for (int Height = 0; Height < GRID_HEIGHT; Height++) for (int Index = -8; Index <= 8; Index++) {
        if (Index == 0) continue;
        if (IsLineFull(Height, Index)) {
            LINE line = { Height, Index };
            lines.push_back(line);
        }
    }
    return lines;
}

std::vector<int> CGrid::CheckForFullPlanes() {
    std::vector<int> planes;
    for (int j = GRID_HEIGHT - 1; j >= 0; j--) if (IsPlaneFull(j)) planes.push_back(j);
    return planes;
}

void CGrid::Tick(int ms) {
    m_ulTimer += ms;
    if (m_Animation.type == ANIMATION_NONE) {
        m_uiCounter += ms;
        int NoOfDescensions = m_uiCounter/(BLOCK_FALL_TIME/(m_bFallFaster?8:1));
        m_uiCounter %= (BLOCK_FALL_TIME/(m_bFallFaster?8:1));
        if (!m_bFallFaster) m_uiPrevCounter = m_uiCounter;

        m_uiMarkerCounter += ms;
        m_uiMarkerCounter %= 3600;

        for (int i = 0; i < NoOfDescensions; i++) if (!DescendBlocks()) {
            MakeAllStationary();
            m_FullLines = CheckForFullLines();
            m_FullPlanes = CheckForFullPlanes();
            if (m_FullLines.size() > 0) {
                m_Animation.type = ANIMATION_LINE;
                m_Animation.counter = 0;
                m_Animation.duration = LINE_ANIM_DURATION;
                m_iScore += 2 * m_FullLines.size();
                return;
            }
            // Whenever an animation ends, a block has to spawn
            if (!SpawnNewBlock(m_NextShape)) m_bGameOver = true;
        }
    } else {
        m_Animation.counter += ms;

        if (m_Animation.type == ANIMATION_LINE) {
            int NoOfWhiteBlocksHorizontal = GRID_WIDTH * m_Animation.counter / m_Animation.duration;
            int NoOfWhiteBlocksVertical = GRID_DEPTH * m_Animation.counter / m_Animation.duration;
            for (unsigned i = 0; i < m_FullLines.size(); i++) {
                if (m_FullLines[i].index > 0) {     // Z-axis line
                    for (int k = 0; k < NoOfWhiteBlocksVertical; k++) m_Blocks[m_FullLines[i].index-1][m_FullLines[i].height][k].col = WHITE;
                } else {                            // X-axis line
                    for (int x = 0; x < NoOfWhiteBlocksHorizontal; x++) m_Blocks[x][m_FullLines[i].height][-m_FullLines[i].index-1].col = WHITE;
                }
            }
        }

        if (m_Animation.counter >= m_Animation.duration) {
            if (m_Animation.type == ANIMATION_LINE) m_FullLines.clear();
            else if (m_Animation.type == ANIMATION_PLANE) {
                for (unsigned ctr = 0; ctr < m_FullPlanes.size(); ctr++) {
                    int j = m_FullPlanes[ctr];
                    for (int i = 0; i < GRID_WIDTH; i++) for (int k = 0; k < GRID_DEPTH; k++) {
                        m_Blocks[i][j][k].Clear();
                        for (int y = j; y < GRID_HEIGHT - 1; y++) {
                            m_Blocks[i][y][k] = m_Blocks[i][y+1][k];
                            m_Blocks[i][y+1][k].Clear();
                        }
                    }
                }
                m_FullPlanes.clear();
            }

            if (m_Animation.type == ANIMATION_LINE && m_FullPlanes.size() > 0) {
                m_Animation.type = ANIMATION_PLANE;
                m_Animation.counter = m_Animation.counter - m_Animation.duration;
                m_Animation.duration = PLANE_ANIM_DURATION;
                if (m_FullPlanes.size() == 1) m_iScore += 25;
                else if (m_FullPlanes.size() == 2) m_iScore += 125;
                else if (m_FullPlanes.size() == 3) m_iScore += 625;
                else if (m_FullPlanes.size() == 4) m_iScore += 3125;
            } else if (m_Animation.type == ANIMATION_LINE || m_Animation.type == ANIMATION_PLANE) {
                m_Animation.type = ANIMATION_NONE;
                m_uiCounter = m_Animation.counter - m_Animation.duration;
                if (!SpawnNewBlock(m_NextShape)) m_bGameOver = true;
            }
        }
    }
}

bool CGrid::DescendBlocks() {
    if (IsClear()) return false;
    else {
        // Check if no blocks are moving
        bool noBlocksMoving = true;
        ITERATE_THROUGH_BLOCKS if (m_Blocks[i][j][k].moving) {
            noBlocksMoving = false;
        }
        if (noBlocksMoving) return false;

        // Check if it can move down
        ITERATE_THROUGH_BLOCKS if (m_Blocks[i][j][k].moving) {
            if (j == 0) return false;
            if (m_Blocks[i][j - 1][k].col != INVISIBLE && !m_Blocks[i][j - 1][k].moving) return false;
        }

        /* If the function got to here, then the block can safely move down */

        ITERATE_THROUGH_BLOCKS if (m_Blocks[i][j][k].moving) {
            // Set the current blocks to have group 2
            m_Blocks[i][j][k].group = 2;
            // Then copy the block to the one below
            m_Blocks[i][j - 1][k].Set(m_Blocks[i][j][k].col, true, 1, m_Blocks[i][j][k].rot);
            m_Blocks[i][j - 1][k].rot.y -= 1.0f;
        }

        // Now delete all blocks with group = 2
        ITERATE_THROUGH_BLOCKS if (m_Blocks[i][j][k].group == 2) m_Blocks[i][j][k].Clear();
        return true;
    }
}

bool CGrid::MoveBlocksHorizontal(int Dir) {
	ITERATE_THROUGH_BLOCKS {
		// Check if there is space for the block to move
		if (m_Blocks[i][j][k].moving) {
			int NewX = i + Dir;
			if (NewX < 0 || NewX > GRID_WIDTH - 1) return false;
			if (m_Blocks[NewX][j][k].col != INVISIBLE && !m_Blocks[NewX][j][k].moving == true) return false;
		}
	}

	/* If the function got to here, then the block can be moved horizontally */

	if (Dir < 0) {
		ITERATE_THROUGH_BLOCKS {
			if (m_Blocks[i][j][k].moving) {
				int NewX = i + Dir;
				// Set the current blocks to have group 2
				m_Blocks[i][j][k].group = 2;
				// Then copy the block to the one across
				m_Blocks[NewX][j][k].Set(m_Blocks[i][j][k].col, true, 1, m_Blocks[i][j][k].rot);
				m_Blocks[NewX][j][k].rot.x += float(Dir);
			}
		}
	} else {
		ITERATE_THROUGH_BLOCKS_BACKWARDS {
			if (m_Blocks[i][j][k].moving) {
				int NewX = i + Dir;
				// Set the current blocks to have group 2
				m_Blocks[i][j][k].group = 2;
				// Then copy the block to the one across
				m_Blocks[NewX][j][k].Set(m_Blocks[i][j][k].col, true, 1, m_Blocks[i][j][k].rot);
				m_Blocks[NewX][j][k].rot.x += float(Dir);
			}
		}
	}

	// Now delete all blocks with group = 2
	ITERATE_THROUGH_BLOCKS if (m_Blocks[i][j][k].group == 2) m_Blocks[i][j][k].Clear();
	return true;
}

bool CGrid::MoveBlocksVertical(int Dir) {
	ITERATE_THROUGH_BLOCKS {
		// Check if there is space for the block to move
		if (m_Blocks[i][j][k].moving) {
			int NewZ = k + Dir;
			if (NewZ < 0 || NewZ > GRID_DEPTH - 1) return false;
			if (m_Blocks[i][j][NewZ].col != INVISIBLE && !m_Blocks[i][j][NewZ].moving == true) return false;
		}
	}

	/* If the function got to here, then the block can be moved horizontally */

	if (Dir < 0) {
		ITERATE_THROUGH_BLOCKS {
			if (m_Blocks[i][j][k].moving) {
				int NewZ = k + Dir;
				// Set the current blocks to have group 2
				m_Blocks[i][j][k].group = 2;
				// Then copy the block to the one across
				m_Blocks[i][j][NewZ].Set(m_Blocks[i][j][k].col, true, 1, m_Blocks[i][j][k].rot);
				m_Blocks[i][j][NewZ].rot.z += float(Dir);
			}
		}
	} else {
		ITERATE_THROUGH_BLOCKS_BACKWARDS {
			if (m_Blocks[i][j][k].moving) {
				int NewZ = k + Dir;
				// Set the current blocks to have group 2
				m_Blocks[i][j][k].group = 2;
				// Then copy the block to the one across
				m_Blocks[i][j][NewZ].Set(m_Blocks[i][j][k].col, true, 1, m_Blocks[i][j][k].rot);
				m_Blocks[i][j][NewZ].rot.z += float(Dir);
			}
		}
	}

	// Now delete all blocks with group = 2
	ITERATE_THROUGH_BLOCKS if (m_Blocks[i][j][k].group == 2) m_Blocks[i][j][k].Clear();
	return true;
}

bool CGrid::RotateBlocks(int Axis, float Degrees) {
    std::vector<glm::vec3> BlocksHolder;
	glm::vec3 RotationCentre;
	COLOUR CurrentColour = INVISIBLE;
	bool NoneMoving = true;

	ITERATE_THROUGH_BLOCKS if (m_Blocks[i][j][k].moving) {
	    NoneMoving = false;
		RotationCentre = m_Blocks[i][j][k].rot;
		CurrentColour = m_Blocks[i][j][k].col;

		glm::vec3 BlockVector = glm::vec3(float(i) + 0.5f, float(j) + 0.5f, float(k) + 0.5f) - RotationCentre;
		if (Axis == 0) BlockVector = glm::rotateX(BlockVector, glm::radians(-Degrees));
		else if (Axis == 1) BlockVector = glm::rotateY(BlockVector, glm::radians(Degrees));
		else if (Axis == 2) BlockVector = glm::rotateZ(BlockVector, glm::radians(-Degrees));
		BlockVector += RotationCentre;

		// Check if the block can rotate
		if (m_Blocks[(long) floor(BlockVector.x)][(long) floor(BlockVector.y)][(long) floor(BlockVector.z)].col != INVISIBLE &&
			m_Blocks[(long) floor(BlockVector.x)][(long) floor(BlockVector.y)][(long) floor(BlockVector.z)].moving == false) return false;
		if ((long) floor(BlockVector.x) < 0 || (long) floor(BlockVector.y) < 0 || (long) floor(BlockVector.z) < 0 ||
			(long) floor(BlockVector.x) >= GRID_WIDTH || (long) floor(BlockVector.y) >= GRID_HEIGHT || (long) floor(BlockVector.z) >= GRID_DEPTH) return false;

		BlocksHolder.push_back(BlockVector);
	} if (NoneMoving) return false;

	/* If the function got to here, then the block can rotate */

	ITERATE_THROUGH_BLOCKS if (m_Blocks[i][j][k].moving) m_Blocks[i][j][k].Clear();
	for (unsigned i = 0; i < BlocksHolder.size(); i++) {
		m_Blocks[(long) floor(BlocksHolder[i].x)][(long) floor(BlocksHolder[i].y)][(long) floor(BlocksHolder[i].z)].Set(CurrentColour, true, 1, RotationCentre);
	}
	return true;
}

void CGrid::DropBlock() {
    while (DescendBlocks()) {}
}

void CGrid::SetFallFaster(bool FallFaster) {
    m_bFallFaster = FallFaster;
    if (m_bFallFaster) m_uiCounter = 0;
    else m_uiCounter = m_uiPrevCounter;
}

bool CGrid::SpawnNewBlock(COLOUR col) {
    bool HasntLost = true;
    glm::vec3 RotationCentre;
    m_NextShape = COLOUR(rand()%7 + 1);
    switch (col) {
	case CYAN:
		if (m_Blocks[GRID_WIDTH / 2][GRID_HEIGHT - 1][GRID_DEPTH / 2 + 1].col != INVISIBLE ||
			m_Blocks[GRID_WIDTH / 2][GRID_HEIGHT - 1][GRID_DEPTH / 2 + 0].col != INVISIBLE ||
			m_Blocks[GRID_WIDTH / 2][GRID_HEIGHT - 1][GRID_DEPTH / 2 - 1].col != INVISIBLE ||
			m_Blocks[GRID_WIDTH / 2][GRID_HEIGHT - 1][GRID_DEPTH / 2 - 2].col != INVISIBLE)
			HasntLost = false;
		RotationCentre = glm::vec3(float(GRID_WIDTH / 2) + 0.5f, float(GRID_HEIGHT - 1) + 0.5f, float(GRID_DEPTH / 2) + 0.5f);
		m_Blocks[GRID_WIDTH / 2][GRID_HEIGHT - 1][GRID_DEPTH / 2 + 1].Set(CYAN, true, 1, RotationCentre);
		m_Blocks[GRID_WIDTH / 2][GRID_HEIGHT - 1][GRID_DEPTH / 2 + 0].Set(CYAN, true, 1, RotationCentre);
		m_Blocks[GRID_WIDTH / 2][GRID_HEIGHT - 1][GRID_DEPTH / 2 - 1].Set(CYAN, true, 1, RotationCentre);
		m_Blocks[GRID_WIDTH / 2][GRID_HEIGHT - 1][GRID_DEPTH / 2 - 2].Set(CYAN, true, 1, RotationCentre);
		return HasntLost;
		break;
    case BLUE:
		if (m_Blocks[GRID_WIDTH / 2 - 1][GRID_HEIGHT - 2][GRID_DEPTH / 2 + 0].col != INVISIBLE ||
			m_Blocks[GRID_WIDTH / 2 - 1][GRID_HEIGHT - 1][GRID_DEPTH / 2 + 0].col != INVISIBLE ||
			m_Blocks[GRID_WIDTH / 2][GRID_HEIGHT - 1][GRID_DEPTH / 2 + 0].col != INVISIBLE ||
			m_Blocks[GRID_WIDTH / 2][GRID_HEIGHT - 1][GRID_DEPTH / 2 - 1].col != INVISIBLE ||
			m_Blocks[GRID_WIDTH / 2][GRID_HEIGHT - 1][GRID_DEPTH / 2 - 2].col != INVISIBLE)
			HasntLost = false;
		RotationCentre = glm::vec3(float(GRID_WIDTH / 2) + 0.5f, float(GRID_HEIGHT - 1) + 0.5f, float(GRID_DEPTH / 2 - 1) + 0.5f);
		m_Blocks[GRID_WIDTH / 2 - 1][GRID_HEIGHT - 2][GRID_DEPTH / 2 + 0].Set(BLUE, true, 1, RotationCentre);
		m_Blocks[GRID_WIDTH / 2 - 1][GRID_HEIGHT - 1][GRID_DEPTH / 2 + 0].Set(BLUE, true, 1, RotationCentre);
		m_Blocks[GRID_WIDTH / 2][GRID_HEIGHT - 1][GRID_DEPTH / 2 + 0].Set(BLUE, true, 1, RotationCentre);
		m_Blocks[GRID_WIDTH / 2][GRID_HEIGHT - 1][GRID_DEPTH / 2 - 1].Set(BLUE, true, 1, RotationCentre);
		m_Blocks[GRID_WIDTH / 2][GRID_HEIGHT - 1][GRID_DEPTH / 2 - 2].Set(BLUE, true, 1, RotationCentre);
		return HasntLost;
		break;
	case ORANGE:
		for (int i = -1; i < 2; i++) for (int j = -1; j < 2; j++) {
			if (m_Blocks[GRID_WIDTH / 2 + i][GRID_HEIGHT - 1][GRID_DEPTH / 2 + j].col != INVISIBLE ||
				m_Blocks[GRID_WIDTH / 2 + i][GRID_HEIGHT - 1][GRID_DEPTH / 2 + j].col != INVISIBLE ||
				m_Blocks[GRID_WIDTH / 2 + i][GRID_HEIGHT - 1][GRID_DEPTH / 2 + j].col != INVISIBLE)
				HasntLost = false;
		}
		RotationCentre = glm::vec3(float(GRID_WIDTH / 2) + 0.5f, float(GRID_HEIGHT - 1) + 0.5f, float(GRID_DEPTH / 2) + 0.5f);
		for (int i = -1; i < 2; i++) for (int j = -1; j < 2; j++) {
			m_Blocks[GRID_WIDTH / 2 + i][GRID_HEIGHT - 1][GRID_DEPTH / 2 + j].Set(ORANGE, true, 1, RotationCentre);
			m_Blocks[GRID_WIDTH / 2 + i][GRID_HEIGHT - 1][GRID_DEPTH / 2 + j].Set(ORANGE, true, 1, RotationCentre);
			m_Blocks[GRID_WIDTH / 2 + i][GRID_HEIGHT - 1][GRID_DEPTH / 2 + j].Set(ORANGE, true, 1, RotationCentre);
		}
		return HasntLost;
		break;
	case YELLOW:
		for (int i = -1; i < 1; i++) for (int j = 1; j < 3; j++) for (int k = -1; k < 1; k++) {
			if (m_Blocks[GRID_WIDTH / 2 + i][GRID_HEIGHT - j][GRID_DEPTH / 2 + k].col != INVISIBLE ||
				m_Blocks[GRID_WIDTH / 2 + i][GRID_HEIGHT - j][GRID_DEPTH / 2 + k].col != INVISIBLE ||
				m_Blocks[GRID_WIDTH / 2 + i][GRID_HEIGHT - j][GRID_DEPTH / 2 + k].col != INVISIBLE)
				HasntLost = false;
		}
		for (int i = -1; i < 1; i++) for (int j = 1; j < 3; j++) for (int k = -1; k < 1; k++) {
			m_Blocks[GRID_WIDTH / 2 + i][GRID_HEIGHT - j][GRID_DEPTH / 2 + k].Set(YELLOW, true, 1, glm::vec3(0.0f));
			m_Blocks[GRID_WIDTH / 2 + i][GRID_HEIGHT - j][GRID_DEPTH / 2 + k].Set(YELLOW, true, 1, glm::vec3(0.0f));
			m_Blocks[GRID_WIDTH / 2 + i][GRID_HEIGHT - j][GRID_DEPTH / 2 + k].Set(YELLOW, true, 1, glm::vec3(0.0f));
		}
		return HasntLost;
		break;
	case GREEN:
		if (m_Blocks[GRID_WIDTH / 2 + 0][GRID_HEIGHT - 1][GRID_DEPTH / 2 - 1].col != INVISIBLE ||
			m_Blocks[GRID_WIDTH / 2 + 1][GRID_HEIGHT - 1][GRID_DEPTH / 2 - 1].col != INVISIBLE ||
			m_Blocks[GRID_WIDTH / 2 + 0][GRID_HEIGHT - 1][GRID_DEPTH / 2 - 0].col != INVISIBLE ||
			m_Blocks[GRID_WIDTH / 2 + 1][GRID_HEIGHT - 1][GRID_DEPTH / 2 - 0].col != INVISIBLE ||
			m_Blocks[GRID_WIDTH / 2 - 1][GRID_HEIGHT - 2][GRID_DEPTH / 2 - 1].col != INVISIBLE ||
			m_Blocks[GRID_WIDTH / 2 - 0][GRID_HEIGHT - 2][GRID_DEPTH / 2 - 1].col != INVISIBLE ||
			m_Blocks[GRID_WIDTH / 2 - 1][GRID_HEIGHT - 2][GRID_DEPTH / 2 - 0].col != INVISIBLE ||
			m_Blocks[GRID_WIDTH / 2 - 0][GRID_HEIGHT - 2][GRID_DEPTH / 2 - 0].col != INVISIBLE)
			HasntLost = false;
		RotationCentre = glm::vec3(float(GRID_WIDTH / 2) + 0.5f, float(GRID_HEIGHT - 2) + 0.5f, float(GRID_DEPTH / 2 - 1) + 0.5f);
		m_Blocks[GRID_WIDTH / 2 + 0][GRID_HEIGHT - 1][GRID_DEPTH / 2 - 1].Set(GREEN, true, 1, RotationCentre);
		m_Blocks[GRID_WIDTH / 2 + 1][GRID_HEIGHT - 1][GRID_DEPTH / 2 - 1].Set(GREEN, true, 1, RotationCentre);
		m_Blocks[GRID_WIDTH / 2 + 0][GRID_HEIGHT - 1][GRID_DEPTH / 2 - 0].Set(GREEN, true, 1, RotationCentre);
		m_Blocks[GRID_WIDTH / 2 + 1][GRID_HEIGHT - 1][GRID_DEPTH / 2 - 0].Set(GREEN, true, 1, RotationCentre);
		m_Blocks[GRID_WIDTH / 2 - 1][GRID_HEIGHT - 2][GRID_DEPTH / 2 - 1].Set(GREEN, true, 1, RotationCentre);
		m_Blocks[GRID_WIDTH / 2 - 0][GRID_HEIGHT - 2][GRID_DEPTH / 2 - 1].Set(GREEN, true, 1, RotationCentre);
		m_Blocks[GRID_WIDTH / 2 - 1][GRID_HEIGHT - 2][GRID_DEPTH / 2 - 0].Set(GREEN, true, 1, RotationCentre);
		m_Blocks[GRID_WIDTH / 2 - 0][GRID_HEIGHT - 2][GRID_DEPTH / 2 - 0].Set(GREEN, true, 1, RotationCentre);
		return HasntLost;
		break;
	case MAGENTA:
		if (m_Blocks[GRID_WIDTH / 2][GRID_HEIGHT - 1][GRID_DEPTH / 2].col != INVISIBLE) HasntLost = false;
		m_Blocks[GRID_WIDTH / 2][GRID_HEIGHT - 1][GRID_DEPTH / 2].Set(MAGENTA, true, 1, glm::vec3(0.0f));
		return HasntLost;
		break;
	case RED:
		if (m_Blocks[GRID_WIDTH / 2 - 1][GRID_HEIGHT - 2][GRID_DEPTH / 2 + 0].col != INVISIBLE ||
			m_Blocks[GRID_WIDTH / 2][GRID_HEIGHT - 2][GRID_DEPTH / 2 + 0].col != INVISIBLE ||
			m_Blocks[GRID_WIDTH / 2 - 1][GRID_HEIGHT - 1][GRID_DEPTH / 2 + 0].col != INVISIBLE ||
			m_Blocks[GRID_WIDTH / 2][GRID_HEIGHT - 1][GRID_DEPTH / 2 + 0].col != INVISIBLE ||
			m_Blocks[GRID_WIDTH / 2][GRID_HEIGHT - 1][GRID_DEPTH / 2 - 1].col != INVISIBLE ||
			m_Blocks[GRID_WIDTH / 2][GRID_HEIGHT - 1][GRID_DEPTH / 2 - 2].col != INVISIBLE)
			HasntLost = false;
		RotationCentre = glm::vec3(float(GRID_WIDTH / 2) + 0.5f, float(GRID_HEIGHT - 1) + 0.5f, float(GRID_DEPTH / 2 - 1) + 0.5f);
		m_Blocks[GRID_WIDTH / 2 - 1][GRID_HEIGHT - 2][GRID_DEPTH / 2 + 0].Set(RED, true, 1, RotationCentre);
		m_Blocks[GRID_WIDTH / 2][GRID_HEIGHT - 2][GRID_DEPTH / 2 + 0].Set(RED, true, 1, RotationCentre);
		m_Blocks[GRID_WIDTH / 2 - 1][GRID_HEIGHT - 1][GRID_DEPTH / 2 + 0].Set(RED, true, 1, RotationCentre);
		m_Blocks[GRID_WIDTH / 2][GRID_HEIGHT - 1][GRID_DEPTH / 2 + 0].Set(RED, true, 1, RotationCentre);
		m_Blocks[GRID_WIDTH / 2][GRID_HEIGHT - 1][GRID_DEPTH / 2 - 1].Set(RED, true, 1, RotationCentre);
		m_Blocks[GRID_WIDTH / 2][GRID_HEIGHT - 1][GRID_DEPTH / 2 - 2].Set(RED, true, 1, RotationCentre);
		return HasntLost;
		break;
    default:
        return false;
    }
}

bool CGrid::IsClear() {
    ITERATE_THROUGH_BLOCKS if (m_Blocks[i][j][k].col != INVISIBLE) return false;
    return true;
}

void CGrid::MakeAllStationary() {
    ITERATE_THROUGH_BLOCKS {
        m_Blocks[i][j][k].group = 0;
        m_Blocks[i][j][k].moving = false;
        m_Blocks[i][j][k].rot = glm::vec3(0.0f);
    }
}

std::string CGrid::GetTimerAsString() {
    int secs = m_ulTimer/1000 % 60;
    std::string seconds =  std::string(secs<10?"0":"") + ToString(secs);
    std::string mins = ToString(m_ulTimer/60000);
    return mins + std::string(":") + seconds;
}

void CGrid::Reset() {
    ITERATE_THROUGH_BLOCKS m_Blocks[i][j][k].Clear();
    m_uiCounter = BLOCK_FALL_TIME;
    m_uiPrevCounter = m_uiCounter;
    m_bFallFaster = false;
    m_iSelectedAxis = 0;
    m_Animation.type = ANIMATION_NONE;
    m_NextShape = COLOUR(rand()%7 + 1);
    m_iScore = 0;
    m_ulTimer = 0;
    m_bGameOver = false;
}
