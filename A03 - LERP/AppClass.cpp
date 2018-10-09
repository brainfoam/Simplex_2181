#include "AppClass.h"

std::vector<std::vector<vector3>> stop_list; //The list of lists of stops for the spheres
std::vector<vector3> sphere_positions; //The list of current positions of the spheres
std::vector<int> stop_states; //The destination value of the spheres
float lerp_amt = 0.0f; //The amount of interpolation per sphere
float lerp_incr = .003f; //The interpolation speed

void Application::InitVariables(void)
{
	//Change this to your name and email
	m_sProgrammer = "Joseph Aquiare - jpa3216@rit.edu";
	
	//Set the position and target of the camera
	//(I'm at [0,0,10], looking at [0,0,0] and up is the positive Y axis)
	m_pCameraMngr->SetPositionTargetAndUpward(AXIS_Z * 20.0f, ZERO_V3, AXIS_Y);

	//if the light position is zero move it
	if (m_pLightMngr->GetPosition(1) == ZERO_V3)
		m_pLightMngr->SetPosition(vector3(0.0f, 0.0f, 3.0f));

	//if the background is cornflowerblue change it to black (its easier to see)
	if (vector3(m_v4ClearColor) == C_BLUE_CORNFLOWER)
	{
		m_v4ClearColor = vector4(ZERO_V3, 1.0f);
	}
	
	//if there are no segments create 7
	if(m_uOrbits < 1)
		m_uOrbits = 7;

	m_uOrbits = 7;

	float fSize = 1.0f; //initial size of orbits
	float fRadius = 0.95f; //initial radius of orbits

	//creating a color using the spectrum 
	uint uColor = 650; //650 is Red
	//prevent division by 0
	float decrements = 250.0f / (m_uOrbits > 1? static_cast<float>(m_uOrbits - 1) : 1.0f); //decrement until you get to 400 (which is violet)
	/*
		This part will create the orbits, it start at 3 because that is the minimum subdivisions a torus can have
	*/
	uint uSides = 3; //start with the minimal 3 sides
	for (uint i = uSides; i < m_uOrbits + uSides; i++)
	{
		vector3 v3Color = WaveLengthToRGB(uColor); //calculate color based on wavelength
		m_shapeList.push_back(m_pMeshMngr->GenerateTorus(fSize, fSize - 0.1f, 3, i, v3Color)); //generate a custom torus and add it to the meshmanager

		//Generate the stop list of vector3 vectors
		std::vector<vector3> stop_list_c;
		float step = (2.0f * PI) / float(i);
		float c, s;
		for (float theta = 0.0f; theta < 2.0f * PI; theta += step) {
			c = cos(theta);
			s = sin(theta);
			vector3 point = vector3(c * fSize, s * fSize, 0);
			stop_list_c.push_back(point);
		}

		stop_list.push_back(stop_list_c);
		sphere_positions.push_back(stop_list_c[0]);
		stop_states.push_back(1);

		//End stop list generation
		fSize += 0.5f; //increment the size for the next orbit
		uColor -= static_cast<uint>(decrements); //decrease the wavelength
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

	matrix4 m4View = m_pCameraMngr->GetViewMatrix(); //view Matrix
	matrix4 m4Projection = m_pCameraMngr->GetProjectionMatrix(); //Projection Matrix
	matrix4 m4Offset = IDENTITY_M4; //offset of the orbits, starts as the global coordinate system
	/*
		The following offset will orient the orbits as in the demo, start without it to make your life easier.
	*/
	//m4Offset = glm::rotate(IDENTITY_M4, 1.5708f, AXIS_Z);

	// draw a shapes

	for (uint i = 0; i < m_uOrbits; ++i)
	{
		m_pMeshMngr->AddMeshToRenderList(m_shapeList[i], glm::rotate(m4Offset, 1.5708f, AXIS_X));

		//calculate the current position
		/////////////////////////////////

		vector3 original_pos = stop_list[i][stop_states[i]];
		vector3 goal_pos;

		if (stop_states[i] < stop_list[i].size() - 1)
		{
			goal_pos = stop_list[i][stop_states[i] + 1];
		}
		else
		{
			goal_pos = stop_list[i][0];
		}

		if (glm::distance(original_pos, goal_pos) < 1.0f)
		{
			stop_states[i]++;
			if (stop_states[i] >= stop_list[i].size())
			{
				stop_states[i] = 0;
			}
			if (stop_states[i] < stop_list[i].size() - 1)
			{
				goal_pos = stop_list[i][stop_states[i] + 1];
			}
			else
			{
				goal_pos = stop_list[i][0];
			}
		}

		sphere_positions[i] = glm::lerp(original_pos, goal_pos, lerp_amt);

		lerp_amt += lerp_incr;

		/////////////////////////////////

		matrix4 m4Model = glm::translate(m4Offset, sphere_positions[i]);

		//draw spheres
		m_pMeshMngr->AddSphereToRenderList(m4Model * glm::scale(vector3(0.1)), C_WHITE);
	}

	if (lerp_amt >= 1.0f)
	{
		lerp_amt = 0.0f;
		for (uint i = 0; i < m_uOrbits; i++)
		{
			stop_states[i]++;
			if (stop_states[i] >= stop_list[i].size())
			{
				stop_states[i] = 0;
			}
		}
	}

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
	//release GUI
	ShutdownGUI();
}