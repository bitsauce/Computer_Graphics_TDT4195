from scipy import misc
import numpy as np
import matplotlib.pyplot as plt

# Returns the greyscale verison of RGB image 'img' using the average of the RGB components
def toGreyscaleAverage(img):
	w = len(img) # Width of img
	h = len(img[0]) # Height of img
	img = img.astype(float) # Convert image to float
	newImg = np.zeros((w, h), dtype=np.uint8) # Create new image
	
	# For every pixel in img
	for y in range(h):
		for x in range(w):
			pixel = img[y][x] 
			newImg[y][x] = np.clip((pixel[0] + pixel[1] + pixel[2]) / 3.0, 0, 255) # Calculate and store average of the RGB components (clip to make sure its in [0, 255] range (it should be anyways but...))
			
	# Return our greyscale image
	return newImg

# Returns the greyscale verison of RGB image 'img' using luminance preserving weights
def toGreyscale(img):
	w = len(img) # Width of img
	h = len(img[0]) # Height of img
	newImg = np.zeros((w, h), dtype=np.uint8) # Create new image
	
	# For every pixel in img
	for y in range(h):
		for x in range(w):
			pixel = img[y][x]
			newImg[y][x] = 0.2126 * pixel[0] + 0.7152 * pixel[1] + 0.0722 * pixel[2] # Calculate the greyscale level using the weights
			
	# Return our greyscale image
	return newImg	

# Read an image
#img = misc.imread("images/4.1.07-jelly-beans.tiff")
img = misc.imread("images/4.2.06-lake.tiff")

# Plot the images in a figure
fig = plt.figure()

# Plot the original image
a = fig.add_subplot(1,3,1)
a.set_title('Original')
plt.imshow(img, cmap=plt.cm.gray)

# Plot the 'average' version
a = fig.add_subplot(1,3,2)
a.set_title('Average')
plt.imshow(toGreyscaleAverage(img), cmap=plt.cm.gray)

# Plot the 'weighted' version
a = fig.add_subplot(1,3,3)
a.set_title('Weighted')
plt.imshow(toGreyscale(img), cmap=plt.cm.gray)

# Show the figure
plt.show()