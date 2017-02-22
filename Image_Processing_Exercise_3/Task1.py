from scipy import misc
import numpy as np
import matplotlib.pyplot as plt

#######
#  a) #
#######

# Get global threshold using the global thresholding algorithm
def get_global_threshold(img, t_initial, t_delta):
	t_prev = 0.0
	t = t_initial
	
	# While the new threshold outside some given delta
	while abs(t - t_prev) > t_delta:
		sum_1 = 0; count_1 = 0
		sum_2 = 0; count_2 = 0
		
		# For every pixel
		for y in range(len(img[0])):
			for x in range(len(img)):				
				# Get pixel value
				value = img[x, y]
				if value > t:
					# Add pixel to set 1
					sum_1 += value
					count_1 += 1
				else:
					# Add pixel to set 2
					sum_2 += value
					count_2 += 1
		
		# Store previous treshold
		t_prev = t
		
		# Calculate new threshold using average
		t = (sum_1 / count_1 + sum_2 / count_2) * 0.5
	
	# Return threshold
	return t_prev

img = misc.imread("images/Fig1051(a)(defective_weld).tif", 'L')

# Create figure for global thresholding algorithm
fig = plt.figure()

# Plot original
a = fig.add_subplot(1,2,1)
a.set_title('Original')
plt.imshow(img, cmap=plt.cm.gray)

# Create segmented image
img_segment = np.zeros((len(img), len(img[0])))
threshold = get_global_threshold(img, 127, 1)

# Set pixels that is over the threshold to white and pixels under threshold to black
for y in range(len(img)):
	for x in range(len(img[0])):				
		value = img[y, x]
		if value > threshold:
			img_segment[y, x] = 255
		else:
			img_segment[y, x] = 0

# Plot global threshold
a = fig.add_subplot(1,2,2)
a.set_title('Global Threshold')
plt.imshow(img_segment, cmap=plt.cm.gray)

#######
#  b) #
#######

def regionGrowing(img, seeds, threshold):
	result = np.zeros((len(img), len(img[0]))) # Output image
	neighbours = [(-1, -1), (0, -1), (1, -1), (-1, 0), (1, 0), (-1, 1), (0, 1), (1, 1)] # 8-neighbourhood connectedness
	
	# While there are pixels to process
	while seeds:
		# Pop point and add it to our solution region
		x, y = seeds.pop()
		result[y, x] = 1.0
		
		# For every neighbour pixel
		for dx, dy in neighbours:
			# Make sure the point is inside the image
			if x + dx >= 0 and y + dy >= 0 and x + dx < len(img[0]) and y + dy < len(img):
				# Make sure this this pixel is not already part of our region
				# and that the homogeneity criteria is fulfilled (pixels within a threshold)
				if result[y + dy, x + dx] < 1.0 and abs(img[y, x] - img[y + dy, x + dx]) <= threshold:
					# Add this neighbour to our region
					seeds.append((x + dx, y + dy))
	return result

# Create figure for global thresholding algorithm
fig = plt.figure()

# Plot original
a = fig.add_subplot(1,2,1)
a.set_title('Original')
plt.imshow(img, cmap=plt.cm.gray)

# Hand-picked seeds
seed = [(140, 253), (294, 250), (442, 243), (414, 233)]

# Plot global threshold
a = fig.add_subplot(1,2,2)
a.set_title('Region Growing')
plt.imshow(regionGrowing(img, seed, 8), cmap=plt.cm.gray)

# Show figures
plt.show()