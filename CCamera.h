#ifndef CCAMERA_H
#define CCAMERA_H

class CCamera {
private:
	glm::vec3 m_Position;
	float m_fYaw;
	float m_fPitch;
public:
	CCamera();
	CCamera(glm::vec3 Position);

	void SetYaw(float Yaw) { m_fYaw = Yaw; m_fYaw = m_fYaw - 360.0f * floor(m_fYaw/360.0f); }
	float GetYaw() { return m_fYaw; }
	void SetPitch(float Pitch) { m_fPitch = Pitch > 90.0f ? 90.0f : (Pitch < -90.0f ? -90.0f : Pitch ); }
	float GetPitch() { return m_fPitch; }
	void IncreaseYaw(float Amount) { SetYaw(m_fYaw + Amount); }
	void IncreasePitch(float Amount) { SetPitch(m_fPitch + Amount); }
	void Translate(glm::vec3 Translation) { m_Position += Translation; }
	glm::mat4 GetMatrix();
	void SetPosition(glm::vec3 Position) { m_Position = Position; }
	glm::vec3 GetPosition() { return m_Position; }

	void LookAt(glm::vec3 Location);
};

#endif // CCAMERA_H
