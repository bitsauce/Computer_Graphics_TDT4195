#include "sphere.hpp"

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

// Creates a VAO containing a sphere with a resolution specified by slices and layers, with a radius of 1.

unsigned int createCircleVAO(unsigned int slices, unsigned int layers, glm::vec4 color)
{

	// Calculating how large our buffers have to be
	// The sphere is defined as layers containing rectangles. Each rectangle requires us to draw two triangles
	const unsigned int PRIMITIVES_PER_RECTANGLE = 2;
	const unsigned int triangleCount = slices * layers * PRIMITIVES_PER_RECTANGLE;

	// Allocating buffers
	const unsigned int VERTICES_PER_TRIANGLE = 3;
	const unsigned int COMPONENTS_PER_VERTEX = 3;
	float* vertices = new float[triangleCount * VERTICES_PER_TRIANGLE * COMPONENTS_PER_VERTEX];
	unsigned int* indices = new unsigned int[triangleCount * VERTICES_PER_TRIANGLE];
	float* colors = new float[triangleCount * VERTICES_PER_TRIANGLE * 4]; // Color buffer

	// Slices require us to define a full revolution worth of triangles.
	// Layers only requires angle varying between the bottom and the top (a layer only covers half a circle worth of angles)
	const float degreesPerLayer = 180.0 / (float) layers;
	const float degreesPerSlice = 360.0 / (float) slices;

	// Keeping track of the triangle index in the buffer. 
	// This implementation is fairly naive in the sense that it does not reuse vertices with the index buffer.
	int i = 0;

	// Constructing the sphere one layer at a time
	for(int layer = 0; layer < layers; layer++)
	{
		int nextLayer = layer + 1;

		// Angles between the vector pointing to any point on a particular layer and the negative z-axis
		float currentAngleZDegrees = degreesPerLayer * layer;
		float nextAngleZDegrees = degreesPerLayer * nextLayer;

		// All coordinates within a single layer share z-coordinates. 
		// So we can calculate those of the current and subsequent layer here.
		float currentZ = -cos(toRadians(currentAngleZDegrees));
		float nextZ = -cos(toRadians(nextAngleZDegrees));

		// The row of vertices forms a circle around the vertical diagonal (z-axis) of the sphere.
		// These radii are also constant for an entire layer, so we can precalculate them.
		float radius = sin(toRadians(currentAngleZDegrees));
		float nextRadius = sin(toRadians(nextAngleZDegrees));

		// Now we can move on to constructing individual slices within a layer
		for(int slice = 0; slice < slices; slice++)
		{

			// The direction of the start and the end of the slice in the xy-plane
			float currentSliceAngleDegrees = slice * degreesPerSlice;
			float nextSliceAngleDegrees = (slice + 1) * degreesPerSlice;

			// Determining the direction vector for both the start and end of the slice
			float currentDirectionX = cos(toRadians(currentSliceAngleDegrees));
			float currentDirectionY = sin(toRadians(currentSliceAngleDegrees));

			float nextDirectionX = cos(toRadians(nextSliceAngleDegrees));
			float nextDirectionY = sin(toRadians(nextSliceAngleDegrees));

			// Now we have all information needed to create triangles

			// Triangle 1

			vertices[3 * i + 0] = radius * currentDirectionX;
			vertices[3 * i + 1] = radius * currentDirectionY;
			vertices[3 * i + 2] = currentZ;

			colors[4 * i + 0] = color.x;
			colors[4 * i + 1] = color.y;
			colors[4 * i + 2] = color.z;
			colors[4 * i + 3] = color.w;

			indices[i] = i;
			i++;

			vertices[3 * i + 0] = radius * nextDirectionX;
			vertices[3 * i + 1] = radius * nextDirectionY;
			vertices[3 * i + 2] = currentZ;

			colors[4 * i + 0] = color.x;
			colors[4 * i + 1] = color.y;
			colors[4 * i + 2] = color.z;
			colors[4 * i + 3] = color.w;

			indices[i] = i;
			i++;

			vertices[3 * i + 0] = nextRadius * nextDirectionX;
			vertices[3 * i + 1] = nextRadius * nextDirectionY;
			vertices[3 * i + 2] = nextZ;

			colors[4 * i + 0] = color.x;
			colors[4 * i + 1] = color.y;
			colors[4 * i + 2] = color.z;
			colors[4 * i + 3] = color.w;

			indices[i] = i;
			i++;

			// Triangle 2

			vertices[3 * i + 0] = radius * currentDirectionX;
			vertices[3 * i + 1] = radius * currentDirectionY;
			vertices[3 * i + 2] = currentZ;

			colors[4 * i + 0] = color.x;
			colors[4 * i + 1] = color.y;
			colors[4 * i + 2] = color.z;
			colors[4 * i + 3] = color.w;

			indices[i] = i;
			i++;

			vertices[3 * i + 0] = nextRadius * nextDirectionX;
			vertices[3 * i + 1] = nextRadius * nextDirectionY;
			vertices[3 * i + 2] = nextZ;

			colors[4 * i + 0] = color.x;
			colors[4 * i + 1] = color.y;
			colors[4 * i + 2] = color.z;
			colors[4 * i + 3] = color.w;

			indices[i] = i;
			i++;

			vertices[3 * i + 0] = nextRadius * currentDirectionX;
			vertices[3 * i + 1] = nextRadius * currentDirectionY;
			vertices[3 * i + 2] = nextZ;

			colors[4 * i + 0] = color.x;
			colors[4 * i + 1] = color.y;
			colors[4 * i + 2] = color.z;
			colors[4 * i + 3] = color.w;

			indices[i] = i;
			i++;
		}
	}

	// Sending the created buffers over to OpenGL.
	// Don't forget to modify this to fit the function you created yourself!
	// You will have to include a file which contains the implementation of this function for this to work.
	GLuint vao_id = generateVertexArray(vertices, colors, indices, triangleCount);

	// Cleaning up after ourselves
	delete[] vertices;
	delete[] indices;
	delete[] colors;

	return vao_id;
}