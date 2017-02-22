from scipy import misc
import scipy.ndimage.morphology as morph
import numpy as np
import matplotlib.pyplot as plt

#######
#  a) #
#######

img = misc.imread("images/noisy.tiff", 'L')
img = img.astype(np.int)

# Create figure for noise reduction
fig = plt.figure()

# Plot original
a = fig.add_subplot(1,2,1)
a.set_title('Original')
plt.imshow(img, cmap=plt.cm.gray)

# Create disk structuring element
r = 7
y,x = np.ogrid[-r : r + 1, -r : r + 1]
mask = x**2+y**2 <= r**2
mask = mask.astype(int)

# Create noise free image, using morphological opening and closing
img_morphed = morph.binary_closing(morph.binary_opening(img, structure=mask), structure=mask)

# Plot noise free version
a = fig.add_subplot(1,2,2)
a.set_title('Noise free')
plt.imshow(img_morphed, cmap=plt.cm.gray)

#######
#  b) #
#######

def distanceTransform(img):
	# Initialize distance transform image to 'img'
	img_dist = img.astype(float)
	count = 0
	
	# While there are 1s in input image
	while not np.all(np.equal(img, 0)):
		# Erode the input image using 3x3 square structuring element
		img = morph.binary_erosion(img, structure=np.ones((3, 3)))	
		
		# Add the eroded image to our cumulative distance transform image
		img_dist += img.astype(float)
		
		# Increase the count
		count += 1
	
	# Normalize the cumulative distance transform image and return
	img_dist = img_dist / count;
	return img_dist

# Create figure for distance transformation
fig = plt.figure()

# Plot original
a = fig.add_subplot(1,2,1)
a.set_title('Original')
plt.imshow(img_morphed, cmap=plt.cm.gray)

# Plot distance transformation of img_morphed from part a)
a = fig.add_subplot(1,2,2)
a.set_title('Distance transform')
plt.imshow(distanceTransform(img_morphed), cmap=plt.cm.gray)

#######
#  c) #
#######

def boundaryExtraction(img):
	return img - morph.binary_erosion(img, structure=np.ones((3, 3)))	
	
# Create figure for distance transformation
fig = plt.figure()

# Plot original
a = fig.add_subplot(1,2,1)
a.set_title('Original')
plt.imshow(img_morphed, cmap=plt.cm.gray)

# Plot the boundary of img_morphed from part a)
a = fig.add_subplot(1,2,2)
a.set_title('Boundary extraction')
plt.imshow(boundaryExtraction(img_morphed), cmap=plt.cm.gray)

# Show figures
plt.show()