#include "AppClass.h"

vector3 v3CurrentPos;
static float fTimer = 0;
//int incr = 0;
//const float fMoveSpeed = .8;
//const float fLerpAmt = .06f;

//-----------------------------
float curr_lerp = 0.0;
float lerp_incr = 0.01;
int goal = 1;
//-----------------------------

void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Joseph Aquiare - jpa3216@g.rit.edu";

	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUpward(vector3(5.0f,3.0f,15.0f), ZERO_V3, AXIS_Y);

	m_pModel = new Simplex::Model();
	m_pModel->Load("Sorted\\WallEye.bto");
	
	m_stopsList.push_back(vector3(-4.0f, -2.0f, 5.0f));
	m_stopsList.push_back(vector3(1.0f, -2.0f, 5.0f));

	m_stopsList.push_back(vector3(-3.0f, -1.0f, 3.0f));
	m_stopsList.push_back(vector3(2.0f, -1.0f, 3.0f));

	m_stopsList.push_back(vector3(-2.0f, 0.0f, 0.0f));
	m_stopsList.push_back(vector3(3.0f, 0.0f, 0.0f));

	m_stopsList.push_back(vector3(-1.0f, 1.0f, -3.0f));
	m_stopsList.push_back(vector3(4.0f, 1.0f, -3.0f));

	m_stopsList.push_back(vector3(0.0f, 2.0f, -5.0f));
	m_stopsList.push_back(vector3(5.0f, 2.0f, -5.0f));

	m_stopsList.push_back(vector3(1.0f, 3.0f, -5.0f));
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

	// Draw the model
	m_pModel->PlaySequence();

	//Get a timer
		//store the new timer
	static uint uClock = m_pSystem->GenClock(); //generate a new clock for that timer
	fTimer += m_pSystem->GetDeltaTime(uClock); //get the delta time for that timer

	//your code goes here

	//slingshot lerp

	//v3CurrentPos = glm::lerp(v3CurrentPos, m_stopsList[incr % m_stopsList.size()], fLerpAmt);
	//if (fTimer > (incr + 1) * fMoveSpeed)
		//incr++;

	//-----------------------------
	if (goal != 0)
	{
		v3CurrentPos = glm::lerp(m_stopsList[goal - 1], m_stopsList[goal], curr_lerp);
	}
	else
	{
		v3CurrentPos = glm::lerp(m_stopsList[m_stopsList.size() - 1], m_stopsList[goal], curr_lerp);
	}
		
	curr_lerp += lerp_incr;

	if (glm::distance(v3CurrentPos, m_stopsList[goal]) < .1) {
		goal++;
		curr_lerp = 0.0f;
	}

	if (goal >= m_stopsList.size()) {
		goal = 0;
	}
	//-----------------------------
	
	matrix4 m4Model = glm::translate(v3CurrentPos);
	m_pModel->SetModelMatrix(m4Model);

	m_pMeshMngr->Print("\nTimer: ");//Add a line on top
	m_pMeshMngr->PrintLine(std::to_string(fTimer), C_YELLOW);

	// Draw stops
	for (uint i = 0; i < m_stopsList.size(); ++i)
	{
		m_pMeshMngr->AddSphereToRenderList(glm::translate(m_stopsList[i]) * glm::scale(vector3(0.05f)), C_GREEN, RENDER_WIRE);
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
	SafeDelete(m_pModel);
	//release GUI
	ShutdownGUI();
}