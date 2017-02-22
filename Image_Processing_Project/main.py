# This file contains the main program that tries to segment the image and identify the shapes
from scipy import misc
from scipy import signal
from scipy import ndimage
from skimage import transform
from skimage import feature
from skimage import draw
from skimage import filters
import numpy as np
import matplotlib.pyplot as plt

# Import my files
from functions import *
from shapes import *
from solutions import *
from constants import *

# DEBUG: Set variables
if debug:
	current_x = 0
	current_y = 0
	current_board = 0

# Returns a list of values describing how close the color of the input shape is to every shape
def identify_based_on_color(img_channels, img_seg):
	shape_likeness = np.zeros(Shape.count)
	shape_color = get_shape_color(img_channels, img_seg)
	for i in range(1, Shape.count):
		shape = Shape.iterator[i]
		shape_likeness[i] = color_difference(shape_color, shape.color, shape.weights)
	return shape_likeness

# Returns a list of values describing how close the input image is compared to all of the shapes
def identify_based_on_lines(img_edges):
	# Find lines using hough transform
	hspace, theta, dists = transform.hough_line(img_edges)

	# Extract lines
	lines = transform.hough_line_peaks(hspace, theta, dists, num_peaks=10)

	# Convert lines to list of lines
	lines = np.array(list(zip(*lines)), dtype=[('hspace', np.uint64), ('angle', np.float), ('dist', np.float)])

	shape_likeness = np.zeros(Shape.count)
	for i in range(1, Shape.count):
		shape = Shape.iterator[i]
		if shape == Shape.CAKE:
			hval = 0.0
			if len(lines) >= 3:
				hval = abs(((lines[0][0] + lines[1][0]) * 0.5) - lines[2][0])
			if hval >= 5.0:
				shape_likeness[i] = 0.0
			else:
				shape_likeness[i] = 10.0
		else:
			shape_likeness[i] = abs(shape.line_count - len(lines))
	return shape_likeness

# Returns the shape we managed to identify in the input image
def identify_shape(img_channels):
	# Array to store the segmented image (one per channel)
	img_seg_channel = np.zeros((3, 100, 100))

	# Set all scores to 'inf'
	shape_scores = np.full((channel_count, Shape.count), np.inf)

	# For every channel...
	for channel in range(channel_count):
		# Segment the image
		img_seg, f = segment(img_channels[channel], 7, 30)
		
		# If the segmented region is sufficiently small or big, it is probably empty, and does not need to be processed
		if f >= 0.9 or f <= 0.1: continue

		# DEBUG: Print debug info
		if debug: print("PROCESSING [%i, %i, %s, %s]" % (current_x, current_y, get_channel_name(channel), board.solution[current_y][current_x].name))

		# Apply canny edge detection
		img_edges = feature.canny(img_seg)

		# Get shape centroid (based on the edge detection)
		centroid = get_centroid(img_edges) 

		# If centroid is outside our region, invert the image (if the shape is not an arrow, as the centroid of the arrow shape won't be inside the region)
		is_arrow = is_arrow_check(img_edges)
		if img_seg[int(centroid[0]), int(centroid[1])] == 0 and not is_arrow:
			img_seg = 1.0 - img_seg # Invert image

		# Calculate color and shape likeness (returns a list of values, one for every shape)
		color_likeness = identify_based_on_color(img_channels, img_seg)
		shape_likeness = identify_based_on_lines(img_edges)

		# For every shape
		for shape in range(1, Shape.count):
			# Store the score value
			shape_scores[channel, shape] = min(color_likeness[shape] + np.power(3.0, shape_likeness[shape]), shape_scores[channel, shape])

		# If is_arrow is False, it is not an arrow, so set the score to inf
		if not is_arrow: shape_scores[channel, Shape.ARROW.index] = np.inf

		# Store the image segmentation for later use
		img_seg_channel[channel] = img_seg

	# Find the segmentation and shape that gives the closest match (smallest score) to any of the shapes for this tile
	min_score_index = np.argmin(shape_scores)
	channel = min_score_index // Shape.count
	shape_index = min_score_index - channel * Shape.count
	if shape_scores[channel, shape_index] == np.inf: shape_index = 0

	# Get shape object
	shape = Shape.iterator[shape_index]

	# DEBUG: Plot segmentation
	if debug:
		fig = plt.figure("Best segmentation for %s" % current_board.title)
		a = fig.add_subplot(board_height, board_width, 1 + current_x + current_y * board_width)
		a.set_title("%s (%.0f)" % (shape.name, (shape_scores[channel, shape_index])))
		plt.axis('off')
		plt.imshow(img_seg_channel[channel], cmap=plt.cm.gray)

	# Return identified shape and centroid of the shape
	return shape, get_centroid(img_seg_channel[channel])

# Saves the 'solution' to the file given by 'path'
def save_solution(path, solution, blue):
	with open(path, 'w') as f:
		if blue: f.write("BLUE\n")
		else: f.write("RED\n")
		for y in range(board_height):
			for x in range(board_width):
				f.write(spaceless_upper_string(solution[y][x].name))
				f.write(" ")
			f.write("\n")

# Iterate through the boards
for board in Board.iterator:
	# Set current board
	current_board = board

	# Print current board
	print("CURRENT BOARD: %s" % board.title)

	# Load board image
	img = misc.imread(board.path)

	# Split the image into board_width x board_height tiles (each tile is 100x100)
	img_split = np.zeros((board_height, board_width, 3, 100, 100), dtype=np.uint8)
	for y in range(board_height):
		for x in range(board_width):
			for i in range(100):
				for j in range(100):
					for channel in range(channel_count):
						img_split[y, x, channel, i, j] = img[y * 100 + i, x * 100 + j, channel]

	# Perform shape recognition on all tiles
	shapes = [[Shape.NONE for i in range(board_width)] for j in range(board_height)]
	for y in range(board_height):
		for x in range(board_width):
			# DEBUG: Set current x, y
			if debug: current_y = y; current_x = x

			# Try to identify shape in current tile
			shape, centroid = identify_shape(img_split[y, x])

			# If a shape was found, plot its centroid into the output image
			if shape != Shape.NONE:
				r = 3
				for i in range(-r, r + 1):
					for j in range(-r, r + 1):
						if np.sqrt(i * i + j * j) > r: continue
						cy = y * 100 + centroid[0] + i; cx = x * 100 + centroid[1] + j
						if cx >= 0 and cx < len(img[0]) and cy >= 0 and cy < len(img):
							img[int(cy), int(cx)] = 255

			# Store result
			shapes[y][x] = shape

	# Test the result against the actual solution
	board.test_solution(shapes, img)

	# Determine if this board starts with a red or blue tile
	first_cell_color = get_shape_color(img_split[0, 0], np.ones((100, 100)))

	# Save the solution to a file
	save_solution("solutions/" + spaceless_upper_string(board.title), shapes, first_cell_color[2] > first_cell_color[0])

	print("------------------------")

plt.show()