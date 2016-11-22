// Local headers
#include "program.hpp"
#include "gloom/gloom.hpp"
#include "gloom/shader.hpp"

GLuint createTriangleVAO(float *vertices, unsigned char *indices, const int triangleCount)
{
	// Generate and bind Vertex Array Object
	GLuint vaoID, vboID, iboID;
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	// Generate Vertex Buffer Object and upload the vertex data
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, 3 * 3 * triangleCount * sizeof(float), vertices, GL_STATIC_DRAW);

	// Set and enable vertex attribute pointer for the VBO
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	// Generate Index Buffer Object and upload the index data
	glGenBuffers(1, &iboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * triangleCount * sizeof(unsigned char), indices, GL_STATIC_DRAW);

	// Unbind VAO
	glBindVertexArray(0);

	// Return VAO id
	return vaoID;
}

void runProgram(GLFWwindow* window)
{
    // Set GLFW callback mechanism(s)
    glfwSetKeyCallback(window, keyboardCallback);

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
		 0.2f, -0.2f, 0.0f,
		 0.0f,  0.2f, 0.0f,

		 // Triangle 2
		-0.8f, -0.8f, 0.0f,
		 0.0f, -0.9f, 0.0f,
		-0.2f, -0.3f, 0.0f,

		 // Triangle 3
		 0.3f, -0.2f, 0.0f,
		 0.6f, -0.2f, 0.0f,
		 0.6f,  0.2f, 0.0f,

		 // Triangle 4
		 0.3f, 0.4f, 0.0f,
		 0.6f, 0.2f, 0.0f,
		 0.6f, 0.4f, 0.0f,

		 // Triangle 5
		-0.9f, 0.7f, 0.0f,
		-0.7f, 0.2f, 0.0f,
		-0.6f, 0.7f, 0.0f
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
	GLuint vao = createTriangleVAO(vertices, indices, 5);

	// Load our shader
	Gloom::Shader shader;
	shader.attach("../gloom/shaders/simple.vert");
	shader.attach("../gloom/shaders/simple.frag");
	shader.link();

    // Rendering Loop
    while (!glfwWindowShouldClose(window))
    {
        // Clear colour and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw our triangles
		shader.activate();
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
}
