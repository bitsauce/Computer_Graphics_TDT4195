// Local headers
#include "program.hpp"
#include "sceneGraph.hpp"
#include "sphere.hpp"
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
const float moveSpeed = 0.05f;
const float rotationSpeed = 0.1f;

// Camera struct
struct
{
	glm::vec3 position; // Camera position
	float pitch, yaw;   // Camera orientation (pitch and yaw)
} camera;

// The slices and layers of the spheres
#define SPHERE_SLICES 10
#define SPHERE_LAYERS 10

// Sets up the initial model transformation for the nodes in the scene
void initTransformationMatrix(SceneNode *node)
{
	if(node)
	{
		// Create model matrix for this node
		node->currentTransformationMatrix = glm::rotate(node->currentTransformationMatrix, node->rotationX, glm::vec3(1.0f, 0.0f, 0.0f));	// Apply model rotation
		node->currentTransformationMatrix = glm::rotate(node->currentTransformationMatrix, node->rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
		node->currentTransformationMatrix = glm::rotate(node->currentTransformationMatrix, node->rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));
		node->currentTransformationMatrix = glm::scale(node->currentTransformationMatrix, glm::vec3(node->scaleFactor));					// Apply model scale
		node->currentTransformationMatrix = glm::translate(node->currentTransformationMatrix, glm::vec3(node->x, node->y, node->z));		// Apply model translation

		// Init transformation matrix of the children
		for(SceneNode *child : node->children)
		{
			initTransformationMatrix(child);
		}
	}
}


// Creates the solar system and returns the root node (sun)
SceneNode *createScene()
{
	// Create sun
	SceneNode *sun = createSceneNode();
	sun->vertexArrayObjectID = createCircleVAO(SPHERE_SLICES, SPHERE_LAYERS, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f));
	sun->rotationDirection = glm::vec3(0.0f, 1.0f, 0.0f);
	sun->rotationSpeedRadians = 0.0f;
	sun->rotationX = PI * 0.5f;

	// Planet 1
	{
		SceneNode *planet = createSceneNode();
		planet->vertexArrayObjectID = createCircleVAO(SPHERE_SLICES, SPHERE_LAYERS, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		planet->x = 10.0f;
		planet->scaleFactor = 0.35f;
		planet->rotationDirection = glm::vec3(0.0f, 0.0f, -1.0f);
		planet->rotationSpeedRadians = 2.0f * PI * 0.1f;
		addChild(sun, planet);

		// Moon 1
		{
			SceneNode *moon = createSceneNode();
			moon->vertexArrayObjectID = createCircleVAO(SPHERE_SLICES, SPHERE_LAYERS, glm::vec4(0.5f, 0.5f, 1.0f, 1.0f));
			moon->x = 5.0f;
			moon->scaleFactor = 0.5f;
			moon->rotationDirection = glm::vec3(0.0f, 1.0f, 1.0f);
			moon->rotationSpeedRadians = 2.0f * PI * 1.0f;
			addChild(planet, moon);
		}
	}

	// Planet 2
	{
		SceneNode *planet = createSceneNode();
		planet->vertexArrayObjectID = createCircleVAO(SPHERE_SLICES, SPHERE_LAYERS, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		planet->x = 20.0f;
		planet->scaleFactor = 0.25f;
		planet->rotationDirection = glm::vec3(0.0f, 0.2f, 1.0f);
		planet->rotationSpeedRadians = 2.0f * PI * 0.2f;
		addChild(sun, planet);
	}

	// Planet 3
	{
		SceneNode *planet = createSceneNode();
		planet->vertexArrayObjectID = createCircleVAO(SPHERE_SLICES, SPHERE_LAYERS, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		planet->x = 25.0f;
		planet->scaleFactor = 0.3f;
		planet->rotationDirection = glm::vec3(0.0f, 1.0f, 1.0f);
		planet->rotationSpeedRadians = 2.0f * PI * 0.1f;
		addChild(sun, planet);
	}

	// Planet 4
	{
		SceneNode *planet = createSceneNode();
		planet->vertexArrayObjectID = createCircleVAO(SPHERE_SLICES, SPHERE_LAYERS, glm::vec4(0.5f, 0.0f, 1.0f, 1.0f));
		planet->x = 30.0f;
		planet->scaleFactor = 0.2f;
		planet->rotationDirection = glm::vec3(0.0f, -0.2f, -1.0f);
		planet->rotationSpeedRadians = 2.0f * PI * 0.4f;
		addChild(sun, planet);
	}

	// Planet 5
	{
		SceneNode *planet = createSceneNode();
		planet->vertexArrayObjectID = createCircleVAO(SPHERE_SLICES, SPHERE_LAYERS, glm::vec4(0.0f, 1.0f, 1.0f, 1.0f));
		planet->x = 50.0f;
		planet->scaleFactor = 0.8f;
		planet->rotationDirection = glm::vec3(0.0f, -0.001f, 1.0f);
		planet->rotationSpeedRadians = 2.0f * PI * 0.1f;
		addChild(sun, planet);

		// Moon 1
		{
			SceneNode *moon = createSceneNode();
			moon->vertexArrayObjectID = createCircleVAO(SPHERE_SLICES, SPHERE_LAYERS, glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));
			moon->x = 7.0f;
			moon->scaleFactor = 0.5f;
			moon->rotationDirection = glm::vec3(0.0f, 0.0f, 1.0f);
			moon->rotationSpeedRadians = 2.0f * PI * 0.2f;
			addChild(planet, moon);
		}

		// Moon 2
		{
			SceneNode *moon = createSceneNode();
			moon->vertexArrayObjectID = createCircleVAO(SPHERE_SLICES, SPHERE_LAYERS, glm::vec4(0.0f, 1.0f, 0.5f, 1.0f));
			moon->y = 5.0f;
			moon->scaleFactor = 0.35f;
			moon->rotationDirection = glm::vec3(0.0f, 0.2f, 1.0f);
			moon->rotationSpeedRadians = 2.0f * PI * 0.8f;
			addChild(planet, moon);
		}
	}

	initTransformationMatrix(sun);

	return sun;
}

// Updates the scene recursively
void updateScene(SceneNode *node, const float dt)
{
	if(node)
	{
		// Apply rotation to the model matrix
		node->currentTransformationMatrix = glm::rotate(node->rotationSpeedRadians * dt, node->rotationDirection) * node->currentTransformationMatrix;

		// Update children
		for(SceneNode *child : node->children)
		{
			updateScene(child, dt);
		}
	}
}

// Draws the scene recursively
void drawScene(SceneNode *node, std::stack<glm::mat4> *matrixStack, glm::mat4 cumulativeModelTransformation = glm::mat4())
{
	if(node)
	{
		// Apply the model transformation matrix to the topmost matrix on the stack
		glm::mat4 modelViewProjection = peekMatrix(matrixStack) * node->currentTransformationMatrix;

		// This cumulative model transformation is used to apply phong shading correclty
		cumulativeModelTransformation = cumulativeModelTransformation * node->currentTransformationMatrix;

		// Feed the mvp and model matrix to our shader program
		glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(modelViewProjection));
		glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(cumulativeModelTransformation));

		// Draw scene node
		glBindVertexArray(node->vertexArrayObjectID);
		glDrawElements(GL_TRIANGLES, SPHERE_SLICES * SPHERE_LAYERS * 2 * 3, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Push current matrix on stack
		pushMatrix(matrixStack, modelViewProjection);

		// Draw children
		for(SceneNode *child : node->children)
		{
			drawScene(child, matrixStack, cumulativeModelTransformation);
		}

		// Pop current matrix from stack
		popMatrix(matrixStack);
	}
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

	// Create scene
	SceneNode *root = createScene();

	// Load our shader
	Gloom::Shader shader;
	shader.attach("../gloom/shaders/simple.vert");
	shader.attach("../gloom/shaders/simple.frag");
	shader.link();

	// Set initial camera position and orientation
	camera.position.x = -15.0f;
	camera.position.y = 3.2f;
	camera.position.z = 9.3f;
	camera.pitch = 9.0f;
	camera.yaw = 146.5f;

	// Calculate projection matrix
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	glm::mat4 projectionMatrix = glm::perspective(1.0f, (float) width / (float) height, 1.0f, 100.0f);

    // Rendering Loop
    while (!glfwWindowShouldClose(window))
    {
        // Clear colour and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update scene
		updateScene(root, getTimeDeltaSeconds());

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

		glm::mat4 eyeSpaceMatrix(
			right.x, up.x, fwd.x, 0.0f,
			right.y, up.y, fwd.y, 0.0f,
			right.z, up.z, fwd.z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		// Calcualte our projection matrix
		glm::mat4 viewProjectionMatrix;
		viewProjectionMatrix = eyeSpaceMatrix * viewProjectionMatrix;					// mvp = eyeSpaceMatrix
		viewProjectionMatrix = glm::translate(viewProjectionMatrix, -camera.position);	// mvp = eyeSpaceMatrix * centerCameraMatrix
		viewProjectionMatrix = projectionMatrix * viewProjectionMatrix;					// mvp = projectionMatrix * eyeSpaceMatrix * centerCameraMatrix

		// Push our projection matrix onto the matrix stack
		std::stack<glm::mat4> *matrixStack = createEmptyMatrixStack();
		pushMatrix(matrixStack, viewProjectionMatrix);

		// Draw scene
		shader.activate();
		drawScene(root, matrixStack);
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
