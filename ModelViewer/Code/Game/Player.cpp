#include "Player.hpp"
#include "Game/GameCommon.hpp"

Player::Player(Game* owner)
	:Entity(owner)
{
	m_playerCamera = new Camera();
	m_playerCamera->SetPerspectiveView(Window::GetMainWindowInstance()->GetAspect(), 60.f, 0.1f, 100.f);
	m_playerCamera->SetRenderBasis(Vec3(0.f, 0.f, 1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));
}

Player::~Player()
{
}

void Player::Update(float deltaSeconds)
{
	HandleInput(deltaSeconds);
	Movement(deltaSeconds);
	m_playerCamera->SetTransform(m_position, m_orientationDegrees);
}

void Player::Render() const
{
}

Camera* Player::GetCamera()
{
	return m_playerCamera;
}

void Player::HandleInput(float deltaSeconds)
{
	UNUSED(deltaSeconds);

	Vec3 forwardDir = GetModeMatrix().GetIBasis3D().GetNormalized();
	Vec3 rightDir = GetModeMatrix().GetJBasis3D().GetNormalized();
	Vec3 upDir = GetModeMatrix().GetKBasis3D().GetNormalized();

	m_velocity = Vec3::ZERO;

	// MKB
	m_orientationDegrees.m_pitchDegrees += g_theInput->GetCursorClientDelta().y  * 0.075f;
	m_orientationDegrees.m_yawDegrees -= g_theInput->GetCursorClientDelta().x  * 0.075f;

	if (g_theInput->IsKeyDown('W'))
	{
		m_velocity += Vec3(forwardDir.x, forwardDir.y, 0).GetNormalized() * m_movingSpeed;
	}
	if (g_theInput->IsKeyDown('S'))
	{
		m_velocity -= Vec3(forwardDir.x, forwardDir.y, 0).GetNormalized() * m_movingSpeed;
	}
	if (g_theInput->IsKeyDown('A'))
	{
		m_velocity += Vec3(rightDir.x, rightDir.y, 0).GetNormalized() * m_movingSpeed;
	}
	if (g_theInput->IsKeyDown('D'))
	{
		m_velocity -= Vec3(rightDir.x, rightDir.y, 0).GetNormalized() * m_movingSpeed;
	}
	if (g_theInput->IsKeyDown('Q'))
	{
		m_velocity += Vec3(0, 0, m_movingSpeed);
	}
	if (g_theInput->IsKeyDown('E'))
	{
		m_velocity -= Vec3(0, 0, m_movingSpeed);
	}
	if (g_theInput->IsKeyDown(KEYCODE_SHIFT))
	{
		m_velocity *= 10.f;
	}

}

void Player::Movement(float deltaSeconds)
{
	m_orientationDegrees.m_pitchDegrees = Clamp(m_orientationDegrees.m_pitchDegrees, -85.f, 85.f);
	m_position += m_velocity * deltaSeconds;
}
