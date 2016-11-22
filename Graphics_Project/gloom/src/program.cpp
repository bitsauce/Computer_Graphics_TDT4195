// Local headers
#include "program.hpp"
#include "sceneGraph.hpp"
#include "shapes.hpp"
#include "gloom/gloom.hpp"
#include "gloom/shader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sstream>
#include <string>
#include <fstream>
#include <utility>

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

// Sets up the initial model transformation for the nodes in the scene
void initTransformationMatrix(SceneNode *node)
{
	if(node)
	{
		// Create model matrix for this node
		node->currentTransformationMatrix = glm::mat4();
		node->currentTransformationMatrix = glm::rotate(node->currentTransformationMatrix, node->rotationX, glm::vec3(1.0f, 0.0f, 0.0f));	// Apply model rotation
		node->currentTransformationMatrix = glm::rotate(node->currentTransformationMatrix, node->rotationY, glm::vec3(0.0f, 1.0f, 0.0f));
		node->currentTransformationMatrix = glm::rotate(node->currentTransformationMatrix, node->rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));
		node->currentTransformationMatrix = glm::translate(node->currentTransformationMatrix, glm::vec3(node->x, node->y, node->z));		// Apply model translation
		node->currentTransformationMatrix = glm::scale(node->currentTransformationMatrix, glm::vec3(node->scaleFactor));					// Apply model scale

		// Init transformation matrix of the children
		for(SceneNode *child : node->children)
		{
			initTransformationMatrix(child);
		}
	}
}

// Get shape type by shape name
const Shape getShapeByName(const std::string &name)
{
	if(name == "NONE") return SHAPE_NONE;
	if(name == "TRIANGLE") return TRIANGLE;
	if(name == "PARALLELOGRAM") return PARALLELOGRAM;
	if(name == "ARROW") return ARROW;
	if(name == "HEXAGON_WHITE") return HEXAGON_WHITE;
	if(name == "HEXAGON_BLACK") return HEXAGON_BLACK;
	if(name == "STAR") return STAR;
	if(name == "CAKE") return CAKE;
	printf("Invalid shape 's'\n", name.c_str());
	return SHAPE_NONE;
}

// Tile structure
struct Tile
{
	Shape shape; // Shape type of the tile
	SceneNode *node; // SceneNode of the tile
};

// Create a tile-grid representation of the board
Tile board[BOARD_HEIGHT][BOARD_WIDTH];

// Currently selected shape position
int selectedShapeX = 0;
int selectedShapeY = 0;

// Flag indicating wether or not we have selected a shape
bool shapeSelected = false;

// The move marker node (a yellow quad indicating where we want to move our shape)
SceneNode *moveMarkerNode = 0;

// Move marker position
int moveMarkerX = 0;
int moveMarkerY = 0;

// Animation variables
bool animateMovement = false; // Flag indicating wether or not we are currently animating
glm::vec3 animationFromPosition; // From position
glm::vec3 animationToPosition; // To position
float animationTime = 0.0f; // Current animation time
SceneNode *animationNode = 0; // Current animation scene node

// If no shape is selected, we change the currently selected shape
// If a shape is selected, we move the destination marker
void moveSelection(const int dx, const int dy)
{
	if(!shapeSelected)
	{
		// No shape is selected, find the next shape in the given direction [dx, dy]
		const int startX = selectedShapeX;
		const int startY = selectedShapeY;
		do
		{
			// If we go outside the right edge, jump one step down
			if(selectedShapeX + dx >= BOARD_WIDTH)
			{
				selectedShapeY = (selectedShapeY + 1) % BOARD_HEIGHT;
				selectedShapeX = 0;
			}
			else if(selectedShapeX + dx < 0)
			{
				selectedShapeY = (selectedShapeY - 1) % BOARD_HEIGHT;
				selectedShapeX = BOARD_WIDTH - 1;
			}
			else
			{
				selectedShapeX += dx;
			}

			// If we go outside the bottom edge, jump one step to the right
			if(selectedShapeY + dy >= BOARD_HEIGHT)
			{
				selectedShapeX = (selectedShapeX + 1) % BOARD_WIDTH;
				selectedShapeY = 0;
			}
			else if(selectedShapeY + dy < 0)
			{
				selectedShapeX = (selectedShapeX - 1) % BOARD_WIDTH;
				selectedShapeY = BOARD_HEIGHT - 1;
			}
			else
			{
				selectedShapeY += dy;
			}

			// If we're back to where we started, break
			if(selectedShapeX == startX && selectedShapeY == startY) break;
		}
		while(board[selectedShapeY][selectedShapeX].shape == SHAPE_NONE);
	}
	else
	{
		// A shape is selected, move the destination marker by [dx, dy]
		moveMarkerX += dx;
		moveMarkerY += dy;

		// Update the destination marker transformation matrix, while making sure it wraps around the edges
		if(moveMarkerX >= BOARD_WIDTH)
		{
			moveMarkerNode->currentTransformationMatrix = glm::translate(glm::vec3(-BOARD_WIDTH + 1, 0.0f, 0.0f)) * moveMarkerNode->currentTransformationMatrix;
			moveMarkerX = 0;
		}
		else if(moveMarkerX < 0)
		{
			moveMarkerNode->currentTransformationMatrix = glm::translate(glm::vec3(BOARD_WIDTH - 1, 0.0f, 0.0f)) * moveMarkerNode->currentTransformationMatrix;
			moveMarkerX = BOARD_WIDTH - 1;
		}
		else if(moveMarkerY >= BOARD_HEIGHT)
		{
			moveMarkerNode->currentTransformationMatrix = glm::translate(glm::vec3(0.0f, -BOARD_HEIGHT + 1, 0.0f)) * moveMarkerNode->currentTransformationMatrix;
			moveMarkerY = 0;
		}
		else if(moveMarkerY < 0)
		{
			moveMarkerNode->currentTransformationMatrix = glm::translate(glm::vec3(0.0f, BOARD_HEIGHT - 1, 0.0f)) * moveMarkerNode->currentTransformationMatrix;
			moveMarkerY = BOARD_HEIGHT - 1;
		}
		else
		{
			moveMarkerNode->currentTransformationMatrix = glm::translate(glm::vec3(dx, dy, 0.0f)) * moveMarkerNode->currentTransformationMatrix;
		}
	}
}

// If no shape is selected, this function selects the currently 'hovered' shape.
// If a shape is selected, this function sets up the animation that will move the shape to the destination tile.
void selectShape()
{
	if(!shapeSelected)
	{
		// If there is a shape on this position, mark it as selected
		if(board[selectedShapeY][selectedShapeX].node)
		{
			shapeSelected = true;
			moveMarkerNode->currentTransformationMatrix = glm::translate(glm::vec3(selectedShapeX, selectedShapeY, -0.002f)) * moveMarkerNode->currentTransformationMatrix; // Show move marker
			moveMarkerX = selectedShapeX;
			moveMarkerY = selectedShapeY;
		}
	}
	else
	{
		// If the selected shape position equals the destination position, we un-select the shape
		if(moveMarkerX == selectedShapeX && moveMarkerY == selectedShapeY)
		{
			// Un-select current shape
			shapeSelected = false;

			// Hide marker node
			moveMarkerNode->currentTransformationMatrix = glm::translate(glm::vec3(-moveMarkerX, -moveMarkerY, 0.002f)) * moveMarkerNode->currentTransformationMatrix;
		}
		else if(!board[moveMarkerY][moveMarkerX].node) // If this tile is empty, setup animation variables for movement
		{
			// Un-select current shape
			shapeSelected = false;

			// Hide marker node
			moveMarkerNode->currentTransformationMatrix = glm::translate(glm::vec3(-moveMarkerX, -moveMarkerY, 0.002f)) * moveMarkerNode->currentTransformationMatrix;

			// Setup animation
			animateMovement = true;
			animationFromPosition = glm::vec3(selectedShapeX, selectedShapeY, 0.0f);
			animationToPosition = glm::vec3(moveMarkerX, moveMarkerY, 0.0f);
			animationTime = 0.0f;
			animationNode = board[selectedShapeY][selectedShapeX].node;

			// Swap source and destination tiles
			std::swap(board[moveMarkerY][moveMarkerX].shape, board[selectedShapeY][selectedShapeX].shape);
			std::swap(board[moveMarkerY][moveMarkerX].node, board[selectedShapeY][selectedShapeX].node);

			// Set selected shape to destination
			selectedShapeX = moveMarkerX;
			selectedShapeY = moveMarkerY;
		}
	}
}

// Creates the board using input file 'filepath' and returns the root node (the board)
SceneNode *createScene(const std::string &filepath)
{
	std::ifstream file(filepath);
	std::string line;
	if(getline(file, line))
	{
		// Create board
		SceneNode *boardNode = createSceneNode();
		boardNode->vertexArrayObjectID = createBoard(line == "BLUE");
		boardNode->z = -0.5f; boardNode->x = -4.0f; boardNode->y = -2.5f; // Center node
		boardNode->rotationX = PI * 0.5f;

		// Create move marker
		moveMarkerNode = createSceneNode();
		moveMarkerNode->vertexArrayObjectID = createMoveMarker();
		moveMarkerNode->z = 0.001f;
		addChild(boardNode, moveMarkerNode);

		// Read file line by line, and create shapes at the correct position
		int y = 0;
		while(getline(file, line))
		{
			std::istringstream ss(line);
			std::string shapeName;

			int x = 0;
			while(ss >> shapeName)
			{
				const Shape shape = getShapeByName(shapeName);
				if(shape != SHAPE_NONE)
				{
					// Create shape node
					SceneNode *shapeNode = createSceneNode();
					shapeNode->vertexArrayObjectID = createShape(shape);
					shapeNode->z = -0.250001f;
					shapeNode->x = x + 0.5f;
					shapeNode->y = y + 0.5f;
					shapeNode->scaleFactor = 0.75f;

					// Setup board values
					board[y][x].shape = shape;
					board[y][x].node = shapeNode;

					// Add child
					addChild(boardNode, shapeNode);
				}
				else
				{
					// Setup board values
					board[y][x].shape = SHAPE_NONE;
					board[y][x].node = 0;
				}
				x++;
			}
			y++;
		}

		// Init transformation matrices
		initTransformationMatrix(boardNode);

		// This will make sure a shape is selected from the start
		moveSelection(1, 0);

		// Return root node
		return boardNode;
	}

	return 0;
}

// Updates the scene recursively
void updateAnimation(const float dt)
{
	// If there is a node that needs animating
	if(animateMovement)
	{
		// Interpolate the position of the shape from start position to end position over 1 second
		animationTime += dt;
		glm::vec3 pos = glm::mix(animationFromPosition, animationToPosition, glm::clamp(animationTime, 0.0f, 1.0f));
		animationNode->x = pos.x + 0.5f;
		animationNode->y = pos.y + 0.5f;
		initTransformationMatrix(animationNode); // Update transformation matrix
		animateMovement = animationTime < 1.0f;
	}
}

// Draws the scene recursively
void drawScene(SceneNode *node, std::stack<glm::mat4> *matrixStack)
{
	if(node)
	{
		// Apply the model transformation matrix to the topmost matrix on the stack
		glm::mat4 modelViewProjection = peekMatrix(matrixStack) * node->currentTransformationMatrix;

		// Feed the mvp and model matrix to our shader program
		glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(modelViewProjection));
		glUniform1ui(1, board[selectedShapeY][selectedShapeX].node == node); // True if this shape is 'hovered'

		// Draw scene node
		glBindVertexArray(node->vertexArrayObjectID);
		glDrawElements(GL_TRIANGLES, BOARD_HEIGHT * BOARD_WIDTH * 2 * 24, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// Push current matrix on stack
		pushMatrix(matrixStack, modelViewProjection);

		// Draw children
		for(SceneNode *child : node->children)
		{
			drawScene(child, matrixStack);
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
	SceneNode *root = createScene("../boards/EASY_01");

	// Load our shader
	Gloom::Shader shader;
	shader.attach("../gloom/shaders/simple.vert");
	shader.attach("../gloom/shaders/simple.frag");
	shader.link();

	// Set initial camera position and orientation
	camera.position.x = 0.0f;
	camera.position.y = 4.0f;
	camera.position.z = 7.0f;
	camera.pitch = 20.0f;
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

		// Update scene
		updateAnimation(getTimeDeltaSeconds());

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

	// Handle shape selection and movement
	if(action == GLFW_PRESS && !animateMovement)
	{
		if(key == GLFW_KEY_UP) moveSelection(0, -1);
		else if(key == GLFW_KEY_DOWN) moveSelection(0, 1);
		else if(key == GLFW_KEY_LEFT) moveSelection(-1, 0);
		else if(key == GLFW_KEY_RIGHT) moveSelection(1, 0);
		else if(key == GLFW_KEY_ENTER) selectShape();
	}
}

void cursorPosCallback(GLFWwindow* window, double x, double y)
{
	// Apply mouse movement camera controls
	glm::vec2 cursorPos(x, y);
	camera.yaw -= (previousCursorPos.x - cursorPos.x) * rotationSpeed;
	camera.pitch -= (previousCursorPos.y - cursorPos.y) * rotationSpeed;
	previousCursorPos = cursorPos;
}
