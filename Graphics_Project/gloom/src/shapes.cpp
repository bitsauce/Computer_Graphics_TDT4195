#include "shapes.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Generate VAO using input vertices, colors and indices
GLuint generateVertexArray(float *vertices, float *colors, unsigned int *indices, const unsigned int triangleCount)
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
		vertexData[i * 7 + 3] = colors[i * 4 + 0];
		vertexData[i * 7 + 4] = colors[i * 4 + 1];
		vertexData[i * 7 + 5] = colors[i * 4 + 2];
		vertexData[i * 7 + 6] = colors[i * 4 + 3];
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
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (3 + 4) * sizeof(float), (void*) (3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// Generate Index Buffer Object and upload the index data
	glGenBuffers(1, &iboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	// Unbind VAO
	glBindVertexArray(0);

	// Return VAO id
	return vaoID;
}

// Extrude input triangles and generate a VAO for the extruded model
GLuint generateExtrudedVertexArray(float *vertices, float *colors, unsigned int *indices, const unsigned int triangleCount, const float depth)
{
	// Create continous (xyzrgba) interleaved vertex data pointer
	const unsigned int vertexCount = triangleCount * 3;
	const unsigned int vertexDataSize = (3 + 4) * vertexCount * 2;
	float *vertexData = new float[vertexDataSize];
	for(unsigned int i = 0; i < vertexCount; i++)
	{
		// Write xyz
		vertexData[i * 14 + 0] = vertices[i * 3 + 0];
		vertexData[i * 14 + 1] = vertices[i * 3 + 1];
		vertexData[i * 14 + 2] = vertices[i * 3 + 2];

		// Write rgba
		vertexData[i * 14 + 3] = colors[i * 4 + 0];
		vertexData[i * 14 + 4] = colors[i * 4 + 1];
		vertexData[i * 14 + 5] = colors[i * 4 + 2];
		vertexData[i * 14 + 6] = colors[i * 4 + 3];

		// Write extruded xyz
		vertexData[i * 14 + 7] = vertices[i * 3 + 0];
		vertexData[i * 14 + 8] = vertices[i * 3 + 1];
		vertexData[i * 14 + 9] = vertices[i * 3 + 2] + depth;

		// Write rgba
		vertexData[i * 14 + 10] = colors[i * 4 + 0];
		vertexData[i * 14 + 11] = colors[i * 4 + 1];
		vertexData[i * 14 + 12] = colors[i * 4 + 2];
		vertexData[i * 14 + 13] = colors[i * 4 + 3];
	}

	// Reconstruct our index buffer
	unsigned int *indexData = new unsigned int[triangleCount * 24]; // After extrusion, there will be 8 times as many triangles
	for(int i = 0; i < triangleCount; i++)
	{
		// Bottom
		indexData[i * 24 + 0] = indices[i * 3 + 0] * 2;
		indexData[i * 24 + 1] = indices[i * 3 + 1] * 2;
		indexData[i * 24 + 2] = indices[i * 3 + 2] * 2;

		// Top
		indexData[i * 24 + 3] = indices[i * 3 + 0] * 2 + 1;
		indexData[i * 24 + 4] = indices[i * 3 + 2] * 2 + 1;
		indexData[i * 24 + 5] = indices[i * 3 + 1] * 2 + 1;

		// Side 1
		{
			indexData[i * 24 + 6] = indices[i * 3 + 0] * 2;
			indexData[i * 24 + 7] = indices[i * 3 + 0] * 2 + 1;
			indexData[i * 24 + 8] = indices[i * 3 + 1] * 2;

			indexData[i * 24 + 9] = indices[i * 3 + 0] * 2 + 1;
			indexData[i * 24 + 10] = indices[i * 3 + 1] * 2 + 1;
			indexData[i * 24 + 11] = indices[i * 3 + 1] * 2;
		}

		// Side 2
		{
			indexData[i * 24 + 12] = indices[i * 3 + 1] * 2;
			indexData[i * 24 + 13] = indices[i * 3 + 1] * 2 + 1;
			indexData[i * 24 + 14] = indices[i * 3 + 2] * 2;

			indexData[i * 24 + 15] = indices[i * 3 + 1] * 2 + 1;
			indexData[i * 24 + 16] = indices[i * 3 + 2] * 2 + 1;
			indexData[i * 24 + 17] = indices[i * 3 + 2] * 2;
		}

		// Side 3
		{
			indexData[i * 24 + 18] = indices[i * 3 + 2] * 2;
			indexData[i * 24 + 19] = indices[i * 3 + 0] * 2 + 1;
			indexData[i * 24 + 20] = indices[i * 3 + 0] * 2;

			indexData[i * 24 + 21] = indices[i * 3 + 2] * 2 + 1;
			indexData[i * 24 + 22] = indices[i * 3 + 0] * 2 + 1;
			indexData[i * 24 + 23] = indices[i * 3 + 2] * 2;
		}
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
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, (3 + 4) * sizeof(float), (void*) (3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// Generate Index Buffer Object and upload the index data
	glGenBuffers(1, &iboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangleCount * 24 * sizeof(unsigned int), indexData, GL_STATIC_DRAW);

	// Unbind VAO
	glBindVertexArray(0);

	// Return VAO id
	return vaoID;
}

// Creates the model for 'shape' by extruding a 2D version of the shape
GLuint createShape(const Shape shape)
{
	// Shape variables (set inside the switch-statement)
	int triangleCount = 0;
	float* vertices = 0;
	float r, g, b;

	// Generate the specific vertex buffer for 'shape'
	switch(shape)
	{
		case TRIANGLE:
		{
			// Set shape values
			triangleCount = 1;
			r = 1.0f; g = 0.0f; b = 1.0f;

			// Create vertex buffer
			vertices = new float[triangleCount * 3 * 3];

			vertices[0] = -0.5f;
			vertices[1] = 0.5f;
			vertices[2] = 0.0f;

			vertices[3] = 0.5f;
			vertices[4] = 0.5f;
			vertices[5] = 0.0f;

			vertices[6] = 0.0f;
			vertices[7] = -0.5f;
			vertices[8] = 0.0f;

		}
		break;
		
		case PARALLELOGRAM:
		{
			// Set shape values
			triangleCount = 2;
			r = 0.0f; g = 1.0f; b = 0.0f;

			// Create vertex buffer
			vertices = new float[6 * 3];

			vertices[0] = 0.5f;
			vertices[1] = -0.5f;
			vertices[2] = 0.0f;

			vertices[3] = 0.25f;
			vertices[4] = 0.5f;
			vertices[5] = 0.0f;

			vertices[6] = -0.25f;
			vertices[7] = -0.5f;
			vertices[8] = 0.0f;

			vertices[9] = 0.25f;
			vertices[10] = 0.5f;
			vertices[11] = 0.0f;

			vertices[12] = -0.25f;
			vertices[13] = -0.5f;
			vertices[14] = 0.0f;

			vertices[15] = -0.5f;
			vertices[16] = 0.5f;
			vertices[17] = 0.0f;
		}
		break;

		case ARROW:
		{
			// Set shape values
			triangleCount = 4;
			r = 1.0f; g = 1.0f; b = 0.0f;

			// Create vertex buffer
			vertices = new float[triangleCount * 3 * 3];

			for(int i = 0; i < 2; i++)
			{
				const float f = i == 0 ? 1.0f : -1.0f;

				vertices[i * 18 + 0] = -0.5f * f;
				vertices[i * 18 + 1] = 0.5f;
				vertices[i * 18 + 2] = 0.0f;

				vertices[i * 18 + 3] = -0.3f * f;
				vertices[i * 18 + 4] = 0.5f;
				vertices[i * 18 + 5] = 0.0f;

				vertices[i * 18 + 6] = 0.0f;
				vertices[i * 18 + 7] = -0.5f;
				vertices[i * 18 + 8] = 0.0f;

				vertices[i * 18 + 9] = 0.0f;
				vertices[i * 18 + 10] = -0.5f;
				vertices[i * 18 + 11] = 0.0f;

				vertices[i * 18 + 12] = -0.3f * f;
				vertices[i * 18 + 13] = 0.5f;
				vertices[i * 18 + 14] = 0.0f;

				vertices[i * 18 + 15] = 0.0f;
				vertices[i * 18 + 16] = -0.2f;
				vertices[i * 18 + 17] = 0.0f;
			}
		}
		break;

		case HEXAGON_WHITE:
		{
			// Set shape values
			triangleCount = 6;
			r = 1.0f; g = 1.0f; b = 1.0f;

			// Create vertex buffer
			vertices = new float[triangleCount * 3 * 3];

			float angle = 60.0f;
			for(int i = 0; i < triangleCount; i++, angle += 60.0f)
			{
				vertices[i * 9 + 0] = cos(glm::radians(angle)) * 0.5f;
				vertices[i * 9 + 1] = sin(glm::radians(angle)) * 0.5f;
				vertices[i * 9 + 2] = 0.0f;

				vertices[i * 9 + 3] = cos(glm::radians(angle - 60.0f)) * 0.5f;
				vertices[i * 9 + 4] = sin(glm::radians(angle - 60.0f)) * 0.5f;
				vertices[i * 9 + 5] = 0.0f;

				vertices[i * 9 + 6] = 0.0f;
				vertices[i * 9 + 7] = 0.0f;
				vertices[i * 9 + 8] = 0.0f;
			}
		}
		break;

		case HEXAGON_BLACK:
		{
			// Set shape values
			triangleCount = 6;
			r = 0.0f; g = 0.0f; b = 0.0f;

			// Create vertex buffer
			vertices = new float[triangleCount * 3 * 3];

			float angle = 60.0f;
			for(int i = 0; i < triangleCount; i++, angle += 60.0f)
			{
				vertices[i * 9 + 0] = cos(glm::radians(angle)) * 0.5f;
				vertices[i * 9 + 1] = sin(glm::radians(angle)) * 0.5f;
				vertices[i * 9 + 2] = 0.0f;

				vertices[i * 9 + 3] = cos(glm::radians(angle - 60.0f)) * 0.5f;
				vertices[i * 9 + 4] = sin(glm::radians(angle - 60.0f)) * 0.5f;
				vertices[i * 9 + 5] = 0.0f;

				vertices[i * 9 + 6] = 0.0f;
				vertices[i * 9 + 7] = 0.0f;
				vertices[i * 9 + 8] = 0.0f;
			}
		}
		break;

		case STAR:
		{
			// Set shape values
			triangleCount = 8;
			r = 0.0f; g = 0.0f; b = 1.0f;

			// Create vertex buffer
			vertices = new float[triangleCount * 3 * 3];

			float angle = 18.0f;
			for(int i = 0; i < 5; i++, angle += 72.0f)
			{
				vertices[i * 9 + 0] = cos(glm::radians(angle)) * 0.25f;
				vertices[i * 9 + 1] = sin(glm::radians(angle)) * 0.25f;
				vertices[i * 9 + 2] = 0.0f;

				vertices[i * 9 + 3] = cos(glm::radians(angle - 72.0f)) * 0.25f;
				vertices[i * 9 + 4] = sin(glm::radians(angle - 72.0f)) * 0.25f;
				vertices[i * 9 + 5] = 0.0f;

				vertices[i * 9 + 6] = cos(glm::radians(angle - 36.0f)) * 0.5f;
				vertices[i * 9 + 7] = sin(glm::radians(angle - 36.0f)) * 0.5f;
				vertices[i * 9 + 8] = 0.0f;
			}

			// Fill triangle 1
			vertices[45] = cos(glm::radians(angle)) * 0.25f;
			vertices[46] = sin(glm::radians(angle)) * 0.25f;
			vertices[47] = 0.0f;
			angle += 72.0f;

			vertices[48] = cos(glm::radians(angle)) * 0.25f;
			vertices[49] = sin(glm::radians(angle)) * 0.25f;
			vertices[50] = 0.0f;
			angle += 72.0f;

			vertices[51] = cos(glm::radians(angle)) * 0.25f;
			vertices[52] = sin(glm::radians(angle)) * 0.25f;
			vertices[53] = 0.0f;

			// Fill triangle 2
			vertices[54] = cos(glm::radians(angle)) * 0.25f;
			vertices[55] = sin(glm::radians(angle)) * 0.25f;
			vertices[56] = 0.0f;
			angle += 72.0f;

			vertices[57] = cos(glm::radians(angle)) * 0.25f;
			vertices[58] = sin(glm::radians(angle)) * 0.25f;
			vertices[59] = 0.0f;
			angle += 72.0f;

			vertices[60] = cos(glm::radians(angle)) * 0.25f;
			vertices[61] = sin(glm::radians(angle)) * 0.25f;
			vertices[62] = 0.0f;

			// Fill triangle 3
			vertices[63] = cos(glm::radians(angle)) * 0.25f;
			vertices[64] = sin(glm::radians(angle)) * 0.25f;
			vertices[65] = 0.0f;
			angle += 72.0f;

			vertices[66] = cos(glm::radians(angle)) * 0.25f;
			vertices[67] = sin(glm::radians(angle)) * 0.25f;
			vertices[68] = 0.0f;

			vertices[69] = cos(glm::radians(angle - 72.0f * 3)) * 0.25f;
			vertices[70] = sin(glm::radians(angle - 72.0f * 3)) * 0.25f;
			vertices[71] = 0.0f;
		}
		break;

		case PIE:
		{
			// Set shape values
			triangleCount = 12;
			r = 1.0f; g = 0.0f; b = 0.0f;

			// Create vertex buffer
			vertices = new float[triangleCount * 3 * 3];

			float angle = 180.0f;
			const float f = 270.0f / triangleCount;
			for(int i = 0; i < triangleCount; i++, angle += f)
			{
				vertices[i * 9 + 0] = cos(glm::radians(angle + f)) * 0.5f;
				vertices[i * 9 + 1] = sin(glm::radians(angle + f)) * 0.5f;
				vertices[i * 9 + 2] = 0.0f;

				vertices[i * 9 + 3] = cos(glm::radians(angle)) * 0.5f;
				vertices[i * 9 + 4] = sin(glm::radians(angle)) * 0.5f;
				vertices[i * 9 + 5] = 0.0f;

				vertices[i * 9 + 6] = 0.0f;
				vertices[i * 9 + 7] = 0.0f;
				vertices[i * 9 + 8] = 0.0f;
			}
		}
		break;
	}

	// If triangleCount == 0, it's not a valid shape. Return 0
	if(triangleCount == 0) return 0;

	// Generate color and index buffers for the shape
	float* colors = new float[triangleCount * 3 * 4];
	unsigned int* indices = new unsigned int[triangleCount * 3];

	// Set color values
	for(int i = 0; i < triangleCount * 3; i++)
	{
		colors[i * 4 + 0] = r;
		colors[i * 4 + 1] = g;
		colors[i * 4 + 2] = b;
		colors[i * 4 + 3] = 1.0f;
	}

	// Set index value
	for(int i = 0; i < triangleCount * 3; i++)
	{
		indices[i] = i;
	}

	// Generate extruded vao
	GLuint vaoID = generateExtrudedVertexArray(vertices, colors, indices, triangleCount, 0.25f);

	// Cleaning up after ourselves
	delete[] vertices;
	delete[] indices;
	delete[] colors;

	// Return vao
	return vaoID;
}

// Creates a red and blue checkerboard
GLuint createBoard(const bool startWithBlue)
{
	const unsigned int triangleCount = BOARD_HEIGHT * BOARD_WIDTH * 2;

	// Allocate buffers
	float* vertices = new float[triangleCount * 3 * 3];
	float* colors = new float[triangleCount * 3 * 4];
	unsigned int* indices = new unsigned int[triangleCount * 3];

	int i = 0;
	for(int y = 0; y < BOARD_HEIGHT; y++)
	{
		for(int x = 0; x < BOARD_WIDTH; x++)
		{
			// Create checkerboard color pattern
			float r = 0.75f, g = 0.125f, b = 0.125f;
			if((x + y + startWithBlue) % 2 == 1)
			{
				r = 0.125f; b = 0.75f;
			}

			// Triangle 1
			{
				vertices[i * 3 + 0] = x;
				vertices[i * 3 + 1] = y;
				vertices[i * 3 + 2] = 0.0f;

				colors[i * 4 + 0] = r;
				colors[i * 4 + 1] = g;
				colors[i * 4 + 2] = b;
				colors[i * 4 + 3] = 1.0;

				indices[i] = i++;

				vertices[i * 3 + 0] = x;
				vertices[i * 3 + 1] = y + 1;
				vertices[i * 3 + 2] = 0.0f;

				colors[i * 4 + 0] = r;
				colors[i * 4 + 1] = g;
				colors[i * 4 + 2] = b;
				colors[i * 4 + 3] = 1.0;

				indices[i] = i++;

				vertices[i * 3 + 0] = x + 1;
				vertices[i * 3 + 1] = y;
				vertices[i * 3 + 2] = 0.0f;

				colors[i * 4 + 0] = r;
				colors[i * 4 + 1] = g;
				colors[i * 4 + 2] = b;
				colors[i * 4 + 3] = 1.0;

				indices[i] = i++;
			}

			// Triangle 2
			{
				vertices[i * 3 + 0] = x + 1;
				vertices[i * 3 + 1] = y;
				vertices[i * 3 + 2] = 0.0f;

				colors[i * 4 + 0] = r;
				colors[i * 4 + 1] = g;
				colors[i * 4 + 2] = b;
				colors[i * 4 + 3] = 1.0;

				indices[i] = i++;

				vertices[i * 3 + 0] = x;
				vertices[i * 3 + 1] = y + 1;
				vertices[i * 3 + 2] = 0.0f;

				colors[i * 4 + 0] = r;
				colors[i * 4 + 1] = g;
				colors[i * 4 + 2] = b;
				colors[i * 4 + 3] = 1.0;

				indices[i] = i++;

				vertices[i * 3 + 0] = x + 1;
				vertices[i * 3 + 1] = y + 1;
				vertices[i * 3 + 2] = 0.0f;

				colors[i * 4 + 0] = r;
				colors[i * 4 + 1] = g;
				colors[i * 4 + 2] = b;
				colors[i * 4 + 3] = 1.0;

				indices[i] = i++;
			}
		}
	}

	// Generate vao
	GLuint vaoID = generateExtrudedVertexArray(vertices, colors, indices, triangleCount, 1.0f);

	// Cleaning up after ourselves
	delete[] vertices;
	delete[] indices;
	delete[] colors;

	// Return vao
	return vaoID;
}

// Creates the move marker (a yellow quad)
GLuint createMoveMarker()
{
	const unsigned int triangleCount = 2;

	// Allocate buffers
	float* vertices = new float[triangleCount * 3 * 3];
	float* colors = new float[triangleCount * 3 * 4];
	unsigned int* indices = new unsigned int[triangleCount * 3];
	int i = 0;

	// Triangle 1
	{
		vertices[i * 3 + 0] = 0;
		vertices[i * 3 + 1] = 0;
		vertices[i * 3 + 2] = 0.0f;

		colors[i * 4 + 0] = 0.75f;
		colors[i * 4 + 1] = 0.75f;
		colors[i * 4 + 2] = 0.25f;
		colors[i * 4 + 3] = 1.0;

		indices[i] = i++;

		vertices[i * 3 + 0] = 0;
		vertices[i * 3 + 1] = 1;
		vertices[i * 3 + 2] = 0.0f;

		colors[i * 4 + 0] = 0.75f;
		colors[i * 4 + 1] = 0.75f;
		colors[i * 4 + 2] = 0.25f;
		colors[i * 4 + 3] = 1.0;

		indices[i] = i++;

		vertices[i * 3 + 0] = 1;
		vertices[i * 3 + 1] = 0;
		vertices[i * 3 + 2] = 0.0f;

		colors[i * 4 + 0] = 0.75f;
		colors[i * 4 + 1] = 0.75f;
		colors[i * 4 + 2] = 0.25f;
		colors[i * 4 + 3] = 1.0;

		indices[i] = i++;
	}

	// Triangle 2
	{
		vertices[i * 3 + 0] = 1;
		vertices[i * 3 + 1] = 0;
		vertices[i * 3 + 2] = 0.0f;

		colors[i * 4 + 0] = 0.75f;
		colors[i * 4 + 1] = 0.75f;
		colors[i * 4 + 2] = 0.25f;
		colors[i * 4 + 3] = 1.0;

		indices[i] = i++;

		vertices[i * 3 + 0] = 0;
		vertices[i * 3 + 1] = 1;
		vertices[i * 3 + 2] = 0.0f;

		colors[i * 4 + 0] = 0.75f;
		colors[i * 4 + 1] = 0.75f;
		colors[i * 4 + 2] = 0.25f;
		colors[i * 4 + 3] = 1.0;

		indices[i] = i++;

		vertices[i * 3 + 0] = 1;
		vertices[i * 3 + 1] = 1;
		vertices[i * 3 + 2] = 0.0f;

		colors[i * 4 + 0] = 0.75f;
		colors[i * 4 + 1] = 0.75f;
		colors[i * 4 + 2] = 0.25f;
		colors[i * 4 + 3] = 1.0;

		indices[i] = i++;
	}

	// Generate vao
	GLuint vaoID = generateVertexArray(vertices, colors, indices, triangleCount);

	// Cleaning up after ourselves
	delete[] vertices;
	delete[] indices;
	delete[] colors;

	// Return vao
	return vaoID;
}