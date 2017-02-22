from scipy import misc
import scipy.ndimage.filters as fi
from scipy import signal
import math
import numpy as np
import matplotlib.pyplot as plt

#######
#  a) #
#######

def downsample(img):
	# Get image dimensions
	img_width = len(img)
	img_height = len(img[0])
	
	# Create output image (half width and height of the original image)
	out_img = np.zeros((math.ceil(img_width / 2), math.ceil(img_height / 2), 3))
	
	# For every other row and column
	for y in range(0, img_height, 2):
		for x in range(0, img_width, 2):
			out_img[x // 2, y // 2] = img[x, y] # Sample the image
	
	# Return downsampled image
	return out_img

# Loads and returns input image 'file' as a float image in [0, 1] range
def load_image(file, m):
	img = misc.imread(file, mode=m)
	img = img.astype(float)
	img *= 1.0 / 255.0
	return img

# Load image
img = load_image("images/bricks.tiff", 'RGB')

# Create figure for low-pass filter
fig = plt.figure()

# Plot original
a = fig.add_subplot(1,2,1)
a.set_title('Original')
plt.imshow(img, cmap=plt.cm.gray)

# Plot downsampled image
a = fig.add_subplot(1,2,2)
a.set_title('Downsampled')
plt.imshow(downsample(img), cmap=plt.cm.gray)

#######
#  b) #
#######

# Takes a RGB image 'img' and performs 2D spatial convolution on it using the kernel 'kernel'
# (not particularly fast...)
def convolve(img, kernel):
	img_width = len(img)
	img_height = len(img[0])
	channel_count = len(img[0, 0])
	kernel_width = len(kernel)
	kernel_height = len(kernel[0])
	
	# Pad the kernel to the size of the image
	kernel = np.pad(kernel, (0, img_width - kernel_width), 'constant')
	
	# Create output image
	out_img = np.zeros((img_width, img_height, channel_count))
	
	# For every channel
	for i in range(channel_count):
		# Create an image containing only component 'i' of our original image
		convolved_channel = np.zeros((img_width, img_height))
		for y in range(img_height):
			for x in range(img_width):
				convolved_channel[x, y] = img[x, y, i]
				
		# Convolve channel image
		convolved_channel = np.fft.ifft2(np.fft.fft2(convolved_channel) * np.fft.fft2(kernel)).real
		
		# Put convolution result in output image
		for y in range(img_height):
			for x in range(img_width):
				out_img[x, y, i] = convolved_channel[x, y]
		
	# Return output image
	return out_img

def smooth_image(img, kernel_size):
	# Create our gaussian kernel
	gauss = np.zeros((kernel_size, kernel_size))
	gauss[kernel_size // 2, kernel_size // 2] = 1
	gauss = fi.gaussian_filter(gauss, 1.0)
	
	# Convolve and return
	return convolve(img, gauss)
	
# Create figure for low-pass filter
fig = plt.figure()

# Plot original
a = fig.add_subplot(2,2,1)
a.set_title('Original')
plt.imshow(img, cmap=plt.cm.gray)

# Plot downsampled and smoothed image (kernel_size = 3) 
a = fig.add_subplot(2,2,2)
a.set_title('Kernel size = 3')
plt.imshow(downsample(smooth_image(img, 3)), cmap=plt.cm.gray)

# Plot downsampled and smoothed image (kernel_size = 5) 
a = fig.add_subplot(2,2,3)
a.set_title('Kernel size = 5')
plt.imshow(downsample(smooth_image(img, 5)), cmap=plt.cm.gray)

# Plot downsampled and smoothed image (kernel_size = 7)
a = fig.add_subplot(2,2,4)
a.set_title('Kernel size = 7')
plt.imshow(downsample(smooth_image(img, 7)), cmap=plt.cm.gray)

# Show figures
plt.show()