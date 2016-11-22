// Local headers
#include "program.hpp"
#include "gloom/gloom.hpp"
#include "gloom/shader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Enum of keyboard input actions
enum Action
{
	MOVE_UP,
	MOVE_DOWN,
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_FORWARD,
	MOVE_BACKWARD,
	ACTION_COUNT
};

// Action state list
bool actionState[ACTION_COUNT];

// Value storing the previous cursor position
glm::vec2 previousCursorPos(0.0f);

// Camera movement and rotation speed constants
const float moveSpeed = 0.01f;
const float rotationSpeed = 0.1f;

// Camera struct
struct
{
	glm::vec3 position; // Camera position
	float pitch, yaw;   // Camera orientation (pitch and yaw)
} camera;


GLuint createTriangleVAO(float *vertices, float *color, unsigned char *indices, const unsigned int triangleCount)
{
	// Create continous (xyzrgba) interleaved vertex data pointer
	const unsigned int vertexCount = triangleCount * 3;
	const unsigned int vertexDataSize = (3 + 4) * vertexCount;
	float *vertexData = new float[vertexDataSize];
	for(unsigned int i = 0; i < vertexCount; i++)
	{
		// Write xyz
		vertexData[i * 7 + 0] = vertices[i * 3 + 0];
		vertexData[i * 7 + 1] = vertices[i * 3 + 1];
		vertexData[i * 7 + 2] = vertices[i * 3 + 2];

		// Write rgba
		vertexData[i * 7 + 3] = color[i * 4 + 0];
		vertexData[i * 7 + 4] = color[i * 4 + 1];
		vertexData[i * 7 + 5] = color[i * 4 + 2];
		vertexData[i * 7 + 6] = color[i * 4 + 3];
	}

	// Generate and bind Vertex Array Object
	GLuint vaoID, vboID, iboID;
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	// Generate Vertex Buffer Object and upload the vertex data
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, vertexDataSize * sizeof(float), vertexData, GL_STATIC_DRAW);

	// Set and enable vertex attribute pointers for the VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (3 + 4) * sizeof(float), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (3 + 4) * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// Generate Index Buffer Object and upload the index data
	glGenBuffers(1, &iboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexCount * sizeof(unsigned char), indices, GL_STATIC_DRAW);

	// Unbind VAO
	glBindVertexArray(0);

	// Return VAO id
	return vaoID;
}

void runProgram(GLFWwindow* window)
{
    // Set GLFW callback mechanism(s)
    glfwSetKeyCallback(window, keyboardCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback); // Cursor movement callback added

	// Set cursor input mode to GLFW_CURSOR_DISABLED (locks cursor to window)
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Enable depth (Z) buffer (accept "closest" fragment)
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Configure miscellaneous OpenGL settings
    glEnable(GL_CULL_FACE);

    // Set default colour after clearing the colour buffer
    glClearColor(0.3f, 0.3f, 0.4f, 1.0f);

	// Vertex position
	float vertices[] = {
		 // Triangle 1
		-0.2f, -0.2f, 0.0f,
		 0.2f, -0.2f, -0.25f,
		 0.0f,  0.2f, 0.25f,

		 // Triangle 2
		-0.8f, -0.8f, 0.0f,
		 0.0f, -0.9f, 0.25f,
		-0.2f, -0.3f, 0.25f,

		 // Triangle 3
		 0.3f, -0.2f, -2.0f,
		 0.6f, -0.2f, -2.0f,
		 0.6f,  0.2f, -2.0f,

		 // Triangle 4
		 0.3f, 0.4f, -0.75f,
		 0.6f, 0.2f, -1.25f,
		 0.6f, 0.4f, -1.0f,

		 // Triangle 5
		-0.9f, 0.7f, 0.0f,
		-0.7f, 0.2f, 0.0f,
		-0.6f, 0.7f, 0.0f
	};

	// Color
	float colors[] = {
		// Triangle 1
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,

		// Triangle 2
		1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f,

		// Triangle 3
		0.99f, 0.88f, 0.77f, 0.0f, // NOTE: Alpha does nothing when alpha blending is disabled
		0.66f, 0.33f, 0.77f, 0.5f,
		0.11f, 0.22f, 0.44f, 1.0f,

		// Triangle 4
		0.25f, 0.75f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f,
		0.5f, 0.1f, 0.8f, 1.0f,

		// Triangle 5
		0.75f, 0.75f, 0.75f, 1.0f,
		0.25f, 0.25f, 0.25f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	};

	// Triangle indices
	unsigned char indices[] = {
		0, 1, 2,	// Triangle 1
		3, 4, 5,	// Triangle 2
		6, 7, 8,	// Triangle 3
		9, 10, 11,	// Triangle 4
		12, 13, 14	// Triangle 5
	};

	// Create the Vertex Array Object containing our 5 rectangles
	GLuint vao = createTriangleVAO(vertices, colors, indices, 5);

	// Load our shader
	Gloom::Shader shader;
	shader.attach("../gloom/shaders/simple.vert");
	shader.attach("../gloom/shaders/simple.frag");
	shader.link();

	// Set initial camera position and orientation
	camera.position.z = 4.0f;
	camera.yaw = 90.0f;

	// Calculate projection matrix
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glm::mat4 projectionMatrix = glm::perspective(1.0f, (float) width / (float) height, 1.0f, 100.0f);

    // Rendering Loop
    while (!glfwWindowShouldClose(window))
    {
        // Clear colour and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Calculate the camera's forward, right and up vector from the yaw and pitch
		glm::vec3 fwd;
		fwd.x = cos(glm::radians(camera.pitch)) * cos(glm::radians(camera.yaw));
		fwd.y = sin(glm::radians(camera.pitch));
		fwd.z = cos(glm::radians(camera.pitch)) * sin(glm::radians(camera.yaw));
		glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), fwd));
		glm::vec3 up = glm::cross(fwd, right);

		// Move the camera relative to the direction it is facing
		camera.position += right * float((actionState[MOVE_RIGHT] - actionState[MOVE_LEFT]) * moveSpeed);
		camera.position += up * float((actionState[MOVE_UP] - actionState[MOVE_DOWN]) * moveSpeed);
		camera.position += fwd * float((actionState[MOVE_BACKWARD] - actionState[MOVE_FORWARD]) * moveSpeed);

		// Calcualte our model view projection matrix (in this case, model is just identity)
		glm::mat4 modelViewProjectionMatrix(
			right.x, up.x, fwd.x, 0.0f,
			right.y, up.y, fwd.y, 0.0f,
			right.z, up.z, fwd.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);																// mvp = eyeSpaceMatrix
		modelViewProjectionMatrix = glm::translate(modelViewProjectionMatrix, -camera.position);	// mvp = eyeSpaceMatrix * centerCameraMatrix
		modelViewProjectionMatrix = projectionMatrix * modelViewProjectionMatrix;					// mvp = projectionMatrix * eyeSpaceMatrix * centerCameraMatrix

		// Activate shader
		shader.activate();

		// Feed the mvp matrix to our vertex shader
		glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));

		// Draw triangles
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 5 * 3, GL_UNSIGNED_BYTE, 0);
		glBindVertexArray(0);
		shader.deactivate();

        // Handle other events
        glfwPollEvents();

        // Flip buffers
        glfwSwapBuffers(window);
    }

	shader.destroy();
}

void keyboardCallback(GLFWwindow* window, int key, int scancode,
                      int action, int mods)
{
    // Use escape key for terminating the GLFW window
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

	// Update action states based on key input
	if(key == GLFW_KEY_A) actionState[MOVE_LEFT] = action != GLFW_RELEASE;
	else if(key == GLFW_KEY_D) actionState[MOVE_RIGHT] = action != GLFW_RELEASE;
	else if(key == GLFW_KEY_SPACE) actionState[MOVE_UP] = action != GLFW_RELEASE;
	else if(key == GLFW_KEY_LEFT_SHIFT) actionState[MOVE_DOWN] = action != GLFW_RELEASE;
	else if(key == GLFW_KEY_W) actionState[MOVE_FORWARD] = action != GLFW_RELEASE;
	else if(key == GLFW_KEY_S) actionState[MOVE_BACKWARD] = action != GLFW_RELEASE;
}

void cursorPosCallback(GLFWwindow* window, double x, double y)
{
	// Apply mouse movement camera controls
	glm::vec2 cursorPos(x, y);
	camera.yaw -= (previousCursorPos.x - cursorPos.x) * rotationSpeed;
	camera.pitch -= (previousCursorPos.y - cursorPos.y) * rotationSpeed;
	previousCursorPos = cursorPos;
}
