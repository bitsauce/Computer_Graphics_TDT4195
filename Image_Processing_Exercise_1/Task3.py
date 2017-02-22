from scipy import misc
import numpy as np
import matplotlib.pyplot as plt

# a)
# Takes a greyscale image 'img' and performs 2D spatial convolution on it using the kernel 'kernel'
def convolve(img, kernel):
	# Get dimensions
	width = len(img)
	height = len(img[0])
	kernel_width = len(kernel)
	kernel_height = len(kernel[0])
	
	# Create output image
	outputImage = np.zeros((width, height), dtype=np.float)
	
	# For every pixel in our image
	for y in range(height):
		for x in range(width):
			# Apply the kernel
			acc = 0.0
			for yy in range(kernel_height):
				for xx in range(kernel_width):
					px = x + xx - int(kernel_width/2)
					py = y + yy - int(kernel_height/2)
					if px >= 0 and px < width and py >= 0 and py < height: # Make sure we're sampling inside the image
						acc += img[py][px] * kernel[yy][xx]
			outputImage[y][x] = acc
			
	# Return our convolved image
	return outputImage
	
# b)
# Takes an RGB image 'img' and performs 2D spatial convolution on it using the kernel 'kernel'
def convolveRGB(img, kernel):
	# Get dimensions
	width = len(img)
	height = len(img[0])
	kernel_width = len(kernel)
	kernel_height = len(kernel[0])
	
	# Create output image
	outputImage = np.zeros((width, height, 3), dtype=np.float)
	
	# For every channel (R, G, B)
	for i in range(len(img[0][0])):
		# For every pixel in our image
		for y in range(height):
			for x in range(width):
				# Apply the kernel
				acc = 0.0
				for yy in range(kernel_height):
					for xx in range(kernel_width):
						px = x + xx - int(kernel_width/2)
						py = y + yy - int(kernel_height/2)
						if px >= 0 and px < width and py >= 0 and py < height:
							acc += img[py][px][i] * kernel[yy][xx]
				outputImage[y][x][i] = acc
				
	# Return our convolved image
	return outputImage
	
# Load RGB image
img = misc.imread("images/4.1.07-jelly-beans.tiff")
img = img.astype(float) # Convert values to float
img *= 1.0 / 255.0 # Convert values into [0, 1] range

# Create blur kernel figure
fig = plt.figure()

# Plot original
a = fig.add_subplot(1,3,1)
a.set_title('Original')
plt.imshow(img, cmap=plt.cm.gray)

# A bunch of filters
identity_filter = np.array([[0, 0, 0], [0, 1, 0], [0, 0, 0]])
box_blur_filter = np.array([[1, 1, 1], [1, 1, 1], [1, 0, 1]]) * 1.0 / 9.0
gauss_filter = np.array([[1, 4, 6, 4, 1], [4, 16, 24, 16, 4], [6, 24, 36, 24, 6], [4, 16, 24, 16, 4], [1, 4, 6, 4, 1]]) * 1.0 / 256.0

# Plot img convolved with the 'box blur' kernel
a = fig.add_subplot(1,3,2)
a.set_title('Box blur')
plt.imshow(convolveRGB(img, box_blur_filter), cmap=plt.cm.gray)

# Plot img convolved with the 'gauss' kernel
a = fig.add_subplot(1,3,3)
a.set_title('Gauss')
plt.imshow(convolveRGB(img, gauss_filter), cmap=plt.cm.gray)

# Show figures
plt.show()
	
# Load greyscale image
img = misc.imread("images/5.2.09-aerial.tiff")
img = img.astype(float) # Convert values to float
img *= 1.0 / 255.0 # Convert values into [0, 1] range

# Create sobel figure
fig = plt.figure()

# Plot original
a = fig.add_subplot(2,2,1)
a.set_title('Original')
plt.imshow(img, cmap=plt.cm.gray)

# A bunch of filters
sobel_x_filter = np.array([[1, 0, -1], [2, 0, -2], [1, 0, -1]])
sobel_y_filter = np.array([[1, 2, 1], [0, 0, 0], [-1, -2, -1]])

# Plot img convolved with the 'sobel x' kernel
sx = convolve(img, sobel_x_filter)
a = fig.add_subplot(2,2,2)
a.set_title('sx')
plt.imshow(sx, cmap=plt.cm.gray)

# Plot img convolved with the 'sobel y' kernel
sy = convolve(img, sobel_y_filter)
a = fig.add_subplot(2,2,3)
a.set_title('sy')
plt.imshow(sy, cmap=plt.cm.gray)

# Calculate and show the magnitude of the gradients
s = np.sqrt(sx ** 2 + sy ** 2);
a = fig.add_subplot(2,2,4)
a.set_title('Gradients')
plt.imshow(s, cmap=plt.cm.gray)

# Show figures
plt.show()