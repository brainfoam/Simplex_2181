#include "AppClass.h"
#include "vector"

int invader[8][11] =
{
	{ 0,0,1,0,0,0,0,0,1,0,0 },
	{ 0,0,0,1,0,0,0,1,0,0,0 },
	{ 0,0,1,1,1,1,1,1,1,0,0 },
	{ 0,1,1,0,1,1,1,0,1,1,0 },
	{ 1,1,1,1,1,1,1,1,1,1,1 },
	{ 1,0,1,1,1,1,1,1,1,0,1 },
	{ 1,0,1,0,0,0,0,0,1,0,1 },
	{ 0,0,0,1,1,0,1,1,0,0,0 },
};

int incr = 0;
const float move_amp = 5.0f;
const float move_freq = .04f;
const float x_offset = -5.0f;
const float y_offset = 6.0f;

void Application::InitVariables(void)
{
	int added = 0;
	vector3 left_vec(1.0f, 0.0f, 0.0f);
	vector3 down_vec(0.0f, -1.0f, 0.0f);

	for (int i = 0; i < 11; i++)
	{
		glm::translate(down_vec);
		for (int j = 0; j < 8; j++)
		{
			std::cout << invader[i][j];
			glm::translate(left_vec);
			if (invader[j][i] == 1)
			{
				meshes[added] = new MyMesh();
				meshes[added]->GenerateCube(1.0f, C_BLACK);
				added++;
			}
		}
		std::cout << std::endl;
	}
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the arcball active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();
}

void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	matrix4 m4View = m_pCameraMngr->GetViewMatrix();
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix();
	matrix4 m4Scale = glm::scale(IDENTITY_M4, vector3(1.0f, 1.0f, 1.0f));
	matrix4 m4Translate = glm::translate(IDENTITY_M4, vector3(0.0f, 0.0f, 0.0f));
	matrix4 m4Model = m4Scale * m4Translate;

	vector3 translate_vec(0.0f, 0.0f, 0.0f);
	int added = 0;
	incr++;

	for (int i = 0; i < 11; i++) 
	{
		translate_vec.x = x_offset + i + (move_amp * sin(incr * move_freq));
		for (int j = 0; j < 8; j++) 
		{
			translate_vec.y = y_offset -j + (move_amp * cos(incr * move_freq));

			if (invader[j][i] == 1) 
			{
				m4Translate = glm::translate(IDENTITY_M4, translate_vec);
				m4Model = m4Scale * m4Translate;
				meshes[added]->Render(m4Projection, m4View, m4Model);
				added++;
			}
		}
	}

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	for (int i = 0; i < 46; i++) {
		SafeDelete(meshes[i]);
	}

	//release GUI
	ShutdownGUI();
}