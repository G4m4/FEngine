#include "FEngine.h"

#include "Camera.h"
#include "GameObject.h"
#include "util/Time.h"

FEngine::FEngine()
{

}

void FEngine::Run()
{

	Window window( 800,600, "Vulkan" );

	GameObject gameobject;
	Camera* camera = gameobject.AddComponent<Camera>();
	Renderer renderer(window, *camera);	

	float lastTime = Time::ElapsedSinceStartup();
	float delta = 1.f / 144.f;

	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = delta;

	while ( window.WindowOpen() )
	{
		float time = Time::ElapsedSinceStartup();

		if (Time::ElapsedSinceStartup() - lastTime > delta)
		{
			io.DeltaTime = time - lastTime;
			lastTime = time;

			Input::Update();

			glm::vec2 size = renderer.GetSize();
			camera->aspectRatio = size.x / size.y;
			io.DisplaySize = ImVec2(size.x, size.y);

			ImGui::NewFrame();

			// Statistics window
			{
				ImGui::Begin("Statistics");
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
				ImGui::End();
			}


			ImGui::ShowTestWindow();
			renderer.DrawFrame();
		}
	}
}