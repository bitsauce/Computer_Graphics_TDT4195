# This file contains the Shape class
import numpy as np

class ShapeStruct:
	def __init__(self, index, name, color, weights, line_count):
		self.index = index
		self.name = name # Shape name
		self.color = np.array(color, dtype=np.uint8) # Expected color for the shape
		self.weights = np.array(weights, dtype=np.float) # Weigths used by color_difference
		self.line_count = line_count # Expected line count for the shape

class Shape:
	NONE = ShapeStruct(0, "None", [0, 0, 0], [0.0, 0.0, 0.0], 0)
	TRIANGLE = ShapeStruct(1, "Triangle", [135, 30, 85], [1.0, 0.25, 0.75], 3)
	PARALLELOGRAM = ShapeStruct(2, "Parallelogram", [100, 145, 65], [0.5, 1.0, 0.5], 4)
	ARROW = ShapeStruct(3, "Arrow", [220, 195, 50], [1.0, 0.75, 0.25], 4)
	HEXAGON_WHITE = ShapeStruct(4, "Hexagon White", [180, 180, 180], [1.0, 1.0, 1.0], 6)
	HEXAGON_BLACK = ShapeStruct(5, "Hexagon Black", [40, 40, 40], [1.0, 1.0, 1.0], 6)
	STAR = ShapeStruct(6, "Star", [70, 60, 120], [0.25, 0.25, 1.0], 10)
	CAKE = ShapeStruct(7, "Cake", [210, 70, 70], [1.0, 0.25, 0.25], 2)
	iterator = [NONE, TRIANGLE, PARALLELOGRAM, ARROW, HEXAGON_WHITE, HEXAGON_BLACK, STAR, CAKE] # List of all the shapes
	count = len(iterator) # Shape count