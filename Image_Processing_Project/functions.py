# This file is contains a collection of functions used by 'main.py'
from scipy import misc
from scipy import signal
from scipy import ndimage
from skimage import transform
from skimage import feature
from skimage import draw
import numpy as np
import matplotlib.pyplot as plt
from constants import *

# Segments input greyscale image.
# blur_radius specifies the blur_radius of the median blur
# threshold specifies the threshold the region growing algorithm will use
def segment(img, blur_radius, threshold):
	# Apply median (blur) filter
	if blur_radius > 0: img = ndimage.median_filter(img, blur_radius)

	# Apply the region growing
	img, count = grow_region(img, (50, 50), threshold)

	# Return segmented image
	return img, count / 10000.0

# Returns the minimum angle difference between 'line0' and 'line1'
def angle_difference(line0, line1):
	angle_diff = abs(line0[1] - line1[1]);
	return min(angle_diff, np.pi - angle_diff)

# Returns the centroid of the image
def get_centroid(img):
	center = np.zeros(2)
	count = 0
	for y in range(len(img)):
		for x in range(len(img[0])):
			if img[y, x] > 0.0:
				center[0] += y
				center[1] += x
				count += 1
	if count == 0: return np.full(2, 50.0)
	return center / count

# Calculates the average color of the shape given by 'mask'
def get_shape_color(img_channels, mask):
	color = np.zeros(channel_count)
	count = 0
	for y in range(len(mask)):
		for x in range(len(mask[0])):
			if mask[y, x] > 0.0:
				for channel in range(channel_count):
					color[channel] += img_channels[channel, y, x]
				count += 1
	return color / count

# Returns True if the input shape is an arrow
def is_arrow_check(img_edges):
	# Find lines using hough transform
	hspace, theta, dists = transform.hough_line(img_edges)

	# Extract lines
	lines = transform.hough_line_peaks(hspace, theta, dists, num_peaks=4)

	# Convert lines to list of lines
	lines = np.array(list(zip(*lines)), dtype=[('hspace', np.uint64), ('angle', np.float), ('dist', np.float)])

	if len(lines) != 4: return False

	lines = list(lines)
	dists = []
	while lines:
		found = False
		for i in range(1, len(lines)):
			if angle_difference(lines[0], lines[i]) < 0.1:
				# Found a line parallel to lines[0]
				dists.append(abs(lines[0][2] - lines[i][2]))
				del lines[i]
				del lines[0]
				found = True
				break
		if not found: break


	if len(dists) == 2 and abs(12.0 - dists[0]) < 5.0 and abs(12.0 - dists[1]) < 5.0:
		return True
	return False

# Returns the sum of the cannel-wise difference between 'color0' and 'color1', weighted by 'weights'
def color_difference(color0, color1, weigths=np.ones(channel_count)):
	diff = 0
	for channel in range(channel_count):
		diff += abs(color0[channel] - color1[channel]) * weigths[channel]
	return diff

# Performs region growing on greyscale image 'img', starting at 'initial_seed' up to threshold
def grow_region(img, initial_seed, threshold):
	result = np.zeros((len(img), len(img[0])), dtype=np.uint8) # Output image
	neighbours = [(0, -1), (-1, 0), (1, 0), (0, 1)] # 4-neighbourhood connectedness
	
	# Setup variables
	count = 0
	seeds = [initial_seed]
	initial_value = img[initial_seed[0], initial_seed[1]]

	# While there are pixels to process
	while seeds:
		# Pop point and add it to our solution region
		y, x = seeds.pop()

		# Skip this pixel if it's already part of our region
		if result[y, x] == 255: continue

		# Mark pixel as part of our region
		result[y, x] = 255
		count += 1
		
		# For every neighbour pixel
		for dx, dy in neighbours:
			# Make sure the point is inside the image
			if x + dx >= 0 and y + dy >= 0 and x + dx < len(img[0]) and y + dy < len(img):
				# Make sure that the homogeneity criteria is fulfilled (pixels within a threshold)
				if abs(int(initial_value) - int(img[y + dy, x + dx])) <= threshold:
					# Add this neighbour to our region
					seeds.append((y + dy, x + dx))

	# Output image and the count of pixels in the region
	return result, count

# Returns the color channel name
def get_channel_name(channel):
	if channel == 0: return "red"
	if channel == 1: return "green"
	if channel == 2: return "blue"
	return ""

# Returns a upper-case string where spaces have been replaced with an underscore
def spaceless_upper_string(s):
	return s.upper().replace(" ", "_")
