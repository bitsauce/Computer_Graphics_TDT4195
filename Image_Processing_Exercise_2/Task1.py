from scipy import misc
import numpy as np
import matplotlib.pyplot as plt

#######
#  a) #
#######

# Takes a greyscale image 'img' and performs 2D spatial convolution on it using the kernel 'kernel'
def convolve(img, kernel):
	# Get size of img and kernel
	img_width = len(img)
	img_height = len(img[0])
	kernel_width = len(kernel)
	kernel_height = len(kernel[0])
	
	# Pad the kernel to the size of the image
	kernel = np.pad(kernel, (0, img_width - kernel_width), 'constant')
	
	# Return the convolution, by the convolution theorem
	return np.fft.ifft2(np.fft.fft2(img) * np.fft.fft2(kernel)).real
	
# Returns the magnitude spectrum of input image 'img'
def get_magnitude_spectrum(img):
	F = np.fft.fft2(img)
	F = np.fft.fftshift(F)
	F = np.abs(F)
	F = np.log(F + 1.0)
	return F.real

# Loads and returns input image 'file' as a float image in [0, 1] range
def load_image(file, m):
	img = misc.imread(file, mode=m)
	img = img.astype(float)
	img *= 1.0 / 255.0
	return img
	
# Some filters
identity_filter = np.array([[0, 0, 0], [0, 1, 0], [0, 0, 0]])
edge_filter = np.array([[-1, -1, -1], [-1, 8, -1], [-1, -1, -1]]) * 1.0 / 9.0
box_blur_filter = np.array([[1, 1, 1], [1, 1, 1], [1, 1, 1]]) * 1.0 / 9.0
gauss_filter = np.array([[1, 4, 6, 4, 1], [4, 16, 24, 16, 4], [6, 24, 36, 24, 6], [4, 16, 24, 16, 4], [1, 4, 6, 4, 1]]) * 1.0 / 256.0

# Load image
img = load_image("images/4.1.07-jelly-beans.tiff", 'L')

# Create figure for low-pass filter
fig = plt.figure()

# Plot original
a = fig.add_subplot(2,2,1)
a.set_title('$\mathit{f}$ (original)')
plt.imshow(img, cmap=plt.cm.gray)

# Plot img convolved with the 'box blur' (low-pass) kernel
a = fig.add_subplot(2,2,2)
a.set_title('$\mathit{f} * \mathit{h}$ (low-pass)')
img_convolved = convolve(img, box_blur_filter)
plt.imshow(img_convolved, cmap=plt.cm.gray)

# Plot img magnitude spectrum 
a = fig.add_subplot(2,2,3)
a.set_title('$|\mathcal{F}\{\mathit{f}\}|$ (original)')
plt.imshow(get_magnitude_spectrum(img), cmap=plt.cm.gray)

# Plot img_convolved magnitude spectrum
a = fig.add_subplot(2,2,4)
a.set_title('$|\mathcal{F}\{\mathit{f} * \mathit{g}\}|$ (low-pass)')
plt.imshow(get_magnitude_spectrum(img_convolved), cmap=plt.cm.gray)

# Create figure for high-pass
fig = plt.figure()

# Plot original
a = fig.add_subplot(2,2,1)
a.set_title('$\mathit{f}$ (original)')
plt.imshow(img, cmap=plt.cm.gray)

# Plot img convolved with the 'edge' kernel
a = fig.add_subplot(2,2,2)
a.set_title('$\mathit{f} * \mathit{h}$ (high-pass)')
img_convolved = convolve(img, edge_filter)
plt.imshow(img_convolved, cmap=plt.cm.gray)

# Plot img magnitude spectrum 
a = fig.add_subplot(2,2,3)
a.set_title('$|\mathcal{F}\{\mathit{f}\}|$ (original)')
plt.imshow(get_magnitude_spectrum(img), cmap=plt.cm.gray)

# Plot img_convolved magnitude spectrum
a = fig.add_subplot(2,2,4)
a.set_title('$|\mathcal{F}\{\mathit{f} * \mathit{g}\}|$ (high-pass)')
plt.imshow(get_magnitude_spectrum(img_convolved), cmap=plt.cm.gray)

#######
#  b) #
#######

# Load image
img = load_image("images/4.2.06-lake.tiff", 'L')

def sharpen(img):
	# Sharpen filter
	sharpen_filter = np.array([[0, -1, 0], [-1, 4, -1], [0, -1, 0]])
	
	# Sharpen and return
	return img + convolve(img, sharpen_filter)

def histeq(img):
   # Get image histogram
   imhist, bins = np.histogram(img.flatten(),256,normed=True)
   cdf = imhist.cumsum() # Cumulative distribution function
   cdf = 255 * cdf / cdf[-1] # Normalize

   # Use linear interpolation of cdf to find new pixel values
   im2 = np.interp(img.flatten(),bins[:-1],cdf)
   return im2.reshape(img.shape)

# Create figure for 'sharpen image' plot
fig = plt.figure()

# Plot original
a = fig.add_subplot(1,2,1)
a.set_title('Original')
plt.imshow(img, cmap=plt.cm.gray)

# Plot sharpened image
a = fig.add_subplot(1,2,2)
a.set_title('Sharpened')
plt.imshow(sharpen(img), cmap=plt.cm.gray)

# Create figure for 'sharpen image' plot (histogram equalized)
fig = plt.figure()

# Plot original
a = fig.add_subplot(1,2,1)
a.set_title('Original')
plt.imshow(histeq(img), cmap=plt.cm.gray)

# Plot sharpened image
a = fig.add_subplot(1,2,2)
a.set_title('Sharpened')
plt.imshow(histeq(sharpen(img)), cmap=plt.cm.gray)

#######
#  c) #
#######

# Creates and returns a hybrid image
def hybrid(img1, img2):
	r = 15
	return remove_frequencies(img1, r, True) + remove_frequencies(img2, r, False)

# This function will remove frequencies according to a cirlce mask with radius 'r'
def remove_frequencies(img, r, low_pass):
	img_width = len(img)
	img_height = len(img[0])
	
	mask = np.full((img_width, img_height), 0.0 if low_pass == True else 1.0)
	for y in range(-r, r):
		for x in range(-r, r):
			if x*x + y*y < r*r:
				mask[x, y] = 1.0 if low_pass == True else 0.0
	
	# Return the convolution, by the convolution theorem
	return np.fft.ifft2(np.fft.fft2(img) * mask).real
	
# Load new image
img1 = load_image('images/bush.tiff', 'L')
img2 = load_image('images/clinton.tiff', 'L')

# Create figure for hybrid image
fig = plt.figure()

# Plot original
a = fig.add_subplot(1,1,1)
a.set_title('Hybrid')
plt.imshow(hybrid(img1, img2), cmap=plt.cm.gray)

# Show figures
plt.show()