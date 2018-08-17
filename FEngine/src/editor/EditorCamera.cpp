#include "editor/EditorCamera.h"

std::string EditorCamera::GetName() const { return "Editor camera"; }

void EditorCamera::Update(float delta)
{
	Transform* transform = GetGameobject()->GetComponent<Transform>();
	glm::vec3 position = transform->GetPosition();
	glm::quat rotation = transform->GetRotation();

	// Calculates speed
	float realSpeed = speed;
	if (Keyboard::KeyDown(GLFW_KEY_LEFT_SHIFT))
		realSpeed *= speedMultiplier;

	// Camera goes right
	float rightAxis = 0.f;
	if (Keyboard::KeyDown(GLFW_KEY_D))
		rightAxis += 1.f;
	else if (Keyboard::KeyDown(GLFW_KEY_A))
		rightAxis -= 1.f;
	position += delta * realSpeed * rightAxis * transform->Right();

	// Camera goes up
	float upAxis = 0.f;
	if (Keyboard::KeyDown(GLFW_KEY_E))
		upAxis += 1.f;
	else if (Keyboard::KeyDown(GLFW_KEY_Q))
		upAxis -= 1.f;
	position += delta * realSpeed * upAxis * transform->Up();

	// Camera goes forward
	float forwardAxis = 0.f;
	if (Keyboard::KeyDown(GLFW_KEY_W))
		forwardAxis += 1.f;
	else if (Keyboard::KeyDown(GLFW_KEY_S))
		forwardAxis -= 1.f;
	position += delta * realSpeed * forwardAxis * transform->Forward();


	if (Mouse::KeyDown(Mouse::button1))
	{
		glm::vec2 mouseDelta = Mouse::Delta();

		glm::quat yaw = glm::angleAxis(-sensitivity * mouseDelta.x, up);
		glm::quat pitch = glm::angleAxis(-sensitivity * mouseDelta.y, transform->Right());
		rotation = yaw * pitch * rotation;
	}

	transform->SetPosition(position);
	transform->SetRotation(rotation);
}

void EditorCamera::RenderGui()
{
	Camera::RenderGui();
	ImGui::DragFloat("speed", &speed, 1.f, 0.f, std::numeric_limits<float>::max());
	ImGui::DragFloat("speedMultiplier", &speedMultiplier, 1.f, 0.f, std::numeric_limits<float>::max());
	ImGui::DragFloat("sensitivity", &sensitivity, 0.001f, 0.f, std::numeric_limits<float>::max());
}