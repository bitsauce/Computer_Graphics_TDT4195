# This file contains the Board class and the solutions for every board
from shapes import *
from constants import *
import numpy as np
import matplotlib.pyplot as plt

class BoardStruct:
	def __init__(self, title, path, solution):
		self.title = title # Board title
		self.path = path # Board image file path
		self.solution = solution # Board solution

	def test_solution(self, solution, img):
		# Count # of wrong shapes
		wrong_shape_count = 0
		for y in range(board_height):
			for x in range(board_width):
				if solution[y][x] != self.solution[y][x]:
					print("Wrong shape identified at [%i, %i]. Found %s, expected %s" % (x, y, solution[y][x].name, self.solution[y][x].name))
					wrong_shape_count += 1

		# Print result
		if wrong_shape_count > 0:
			print("RESULT: %i shapes were misidentified" % wrong_shape_count)
		else:
			print("RESULT: All shapes were correctly identified")

		# Plot and show the identified shape names
		fig = plt.figure("%s result" % self.title)
		ax = plt.gca()
		plt.axis('off')
		for y in range(board_height):
			for x in range(board_width):
				if solution[y][x] == self.solution[y][x]:
					plt.text((x * 100 + 50) / len(img[0]), 1.0 - (y * 100 + 15) / len(img), solution[y][x].name, horizontalalignment='center', verticalalignment='center', transform=ax.transAxes)
				else:
					plt.text((x * 100 + 50) / len(img[0]), 1.0 - (y * 100 + 15) / len(img), solution[y][x].name + "\n[INCORRECT]", horizontalalignment='center', verticalalignment='center', transform=ax.transAxes)
		plt.imshow(img, cmap=plt.cm.gray)

class Board:
	EASY_01 = BoardStruct(
					"Easy 01",
					"images/easy01.png",
					[[Shape.NONE, Shape.NONE, Shape.NONE, Shape.PARALLELOGRAM, Shape.ARROW, Shape.NONE, Shape.NONE, Shape.CAKE],
					[Shape.NONE, Shape.PARALLELOGRAM, Shape.NONE, Shape.STAR, Shape.NONE, Shape.HEXAGON_BLACK, Shape.NONE, Shape.NONE],
					[Shape.NONE, Shape.HEXAGON_BLACK, Shape.NONE, Shape.HEXAGON_WHITE, Shape.NONE, Shape.NONE, Shape.STAR, Shape.NONE],
					[Shape.NONE, Shape.NONE, Shape.HEXAGON_WHITE, Shape.HEXAGON_BLACK, Shape.NONE, Shape.NONE, Shape.HEXAGON_WHITE, Shape.NONE],
					[Shape.HEXAGON_BLACK, Shape.HEXAGON_WHITE, Shape.NONE, Shape.NONE, Shape.TRIANGLE, Shape.NONE, Shape.NONE, Shape.CAKE]]
				)

	EASY_02 = BoardStruct(
					"Easy 02",
					"images/easy02.png",
					[[Shape.NONE, Shape.NONE, Shape.CAKE, Shape.NONE, Shape.NONE, Shape.HEXAGON_WHITE, Shape.NONE, Shape.NONE],
					[Shape.NONE, Shape.PARALLELOGRAM, Shape.HEXAGON_WHITE, Shape.NONE, Shape.HEXAGON_BLACK, Shape.HEXAGON_WHITE, Shape.HEXAGON_BLACK, Shape.NONE],
					[Shape.NONE, Shape.STAR, Shape.TRIANGLE, Shape.HEXAGON_BLACK, Shape.HEXAGON_WHITE, Shape.NONE, Shape.HEXAGON_BLACK, Shape.NONE],
					[Shape.NONE, Shape.CAKE, Shape.NONE, Shape.ARROW, Shape.STAR, Shape.NONE, Shape.NONE, Shape.NONE],
					[Shape.NONE, Shape.NONE, Shape.NONE, Shape.PARALLELOGRAM, Shape.NONE, Shape.NONE, Shape.NONE, Shape.NONE]]
				)

	DIFFICULT_01 = BoardStruct(
					"Difficult 01",
					"images/difficult01.png",
					[[Shape.NONE, Shape.NONE, Shape.NONE, Shape.NONE, Shape.NONE, Shape.NONE, Shape.NONE, Shape.NONE],
					[Shape.NONE, Shape.NONE, Shape.NONE, Shape.NONE, Shape.NONE, Shape.NONE, Shape.NONE, Shape.NONE],
					[Shape.NONE, Shape.NONE, Shape.NONE, Shape.NONE, Shape.NONE, Shape.NONE, Shape.NONE, Shape.NONE],
					[Shape.HEXAGON_WHITE, Shape.HEXAGON_WHITE, Shape.HEXAGON_WHITE, Shape.HEXAGON_WHITE, Shape.HEXAGON_BLACK, Shape.HEXAGON_BLACK, Shape.HEXAGON_BLACK, Shape.HEXAGON_BLACK],
					[Shape.CAKE, Shape.PARALLELOGRAM, Shape.STAR, Shape.ARROW, Shape.TRIANGLE, Shape.STAR, Shape.PARALLELOGRAM, Shape.CAKE]]
				)

	DIFFICULT_02 = BoardStruct(
					"Difficult 02",
					"images/difficult02.png",
					[[Shape.NONE, Shape.HEXAGON_WHITE, Shape.NONE, Shape.NONE, Shape.NONE, Shape.NONE, Shape.NONE, Shape.NONE],
					[Shape.HEXAGON_BLACK, Shape.PARALLELOGRAM, Shape.HEXAGON_WHITE, Shape.NONE, Shape.HEXAGON_BLACK, Shape.HEXAGON_BLACK, Shape.PARALLELOGRAM, Shape.CAKE],
					[Shape.HEXAGON_BLACK, Shape.NONE, Shape.NONE, Shape.TRIANGLE, Shape.NONE, Shape.NONE, Shape.NONE, Shape.NONE],
					[Shape.NONE, Shape.NONE, Shape.STAR, Shape.NONE, Shape.NONE, Shape.ARROW, Shape.NONE, Shape.CAKE],
					[Shape.NONE, Shape.NONE, Shape.NONE, Shape.HEXAGON_WHITE, Shape.STAR, Shape.NONE, Shape.HEXAGON_WHITE, Shape.NONE]]
				)

	iterator = [EASY_01, EASY_02, DIFFICULT_01, DIFFICULT_02]