from scipy import misc
import numpy as np
import matplotlib.pyplot as plt

# Takes an image 'img' and returns the inverse of it
def invert(img):
	return 1.0 - img

# Takes an image 'img' and returns a gamma adjusted image (using formula c*{img}^gamma)
def adjustGamma(img, c, gamma):
	return c * (img ** gamma)

# Read image
img = misc.imread("images/4.1.07-jelly-beans.tiff")
img = img.astype(float) # Convert values to float
img *= 1.0 / 255.0 # Convert values into [0, 1] range

# Create inversion figure
fig = plt.figure()

# Plot the original image
a = fig.add_subplot(1,2,1)
a.set_title('Original')
plt.imshow(img, cmap=plt.cm.gray)

# Plot the inverted image
a = fig.add_subplot(1,2,2)
a.set_title('Inverted')
plt.imshow(invert(img), cmap=plt.cm.gray)

# Load grayscale image
img = misc.imread("images/5.1.10-aerial.tiff")
img = img.astype(float) # Convert values to float
img *= 1.0 / 255.0 # Convert values into [0, 1] range

# Create gamma figure
fig = plt.figure()

# Plot original
a = fig.add_subplot(2,3,1)
a.set_title('Original')
plt.imshow(img, cmap=plt.cm.gray)

# Plot with gamma 0.2
a = fig.add_subplot(2,3,2)
a.set_title('Gamma 0.2')
plt.imshow(adjustGamma(img, 1.0, 0.2), cmap=plt.cm.gray)

# Plot with gamma 0.5
a = fig.add_subplot(2,3,3)
a.set_title('Gamma 0.5')
plt.imshow(adjustGamma(img, 1.0, 0.5), cmap=plt.cm.gray)

# Plot with gamma 0.8
a = fig.add_subplot(2,3,4)
a.set_title('Gamma 0.8')
plt.imshow(adjustGamma(img, 1.0, 0.8), cmap=plt.cm.gray)

# Plot with gamma 1.0
a = fig.add_subplot(2,3,5)
a.set_title('Gamma 1.0')
plt.imshow(adjustGamma(img, 1.0, 1.0), cmap=plt.cm.gray)

# Plot with gamma 2.0
a = fig.add_subplot(2,3,6)
a.set_title('Gamma 2.0')
plt.imshow(adjustGamma(img, 1.0, 2.0), cmap=plt.cm.gray)

# Show figures
plt.show()