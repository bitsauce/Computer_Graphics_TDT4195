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
sobel_x_filter = np.array([[1, 0, -1], [2, 0, -2], [1, 0, -1]])
sobel_y_filter = np.array([[1, 2, 1], [0, 0, 0], [-1, -2, -1]])

# Load image
img1 = np.zeros((256, 256), dtype=np.float)
img2 = np.zeros((256, 256), dtype=np.float)

img1[0, 0] = 1.0
img1[0, -10] = 1.0
img1[0, +10] = 1.0

img2[0, 0] = 1.0
img2[-10, 0] = 1.0
img2[+10, 0] = 1.0

# Create figure for low-pass filter
fig = plt.figure()

a = fig.add_subplot(2,2,1)
a.set_title('img1')
plt.imshow(np.fft.fftshift(img1), cmap=plt.cm.gray)

a = fig.add_subplot(2,2,2)
a.set_title('img2')
plt.imshow(np.fft.fftshift(img2), cmap=plt.cm.gray)

ifft_img1 = np.fft.ifft2(img1).real
a = fig.add_subplot(2,2,3)
a.set_title('img1')
plt.imshow(ifft_img1, cmap=plt.cm.gray)

ifft_img2 = np.fft.ifft2(img2).real
a = fig.add_subplot(2,2,4)
a.set_title('img2')
plt.imshow(ifft_img2, cmap=plt.cm.gray)

blend_img = ifft_img1 * 0.5 + ifft_img2 * 0.5

fig = plt.figure()

a = fig.add_subplot(1,2,1)
a.set_title('blend_img')
plt.imshow(blend_img, cmap=plt.cm.gray)

a = fig.add_subplot(1,2,2)
a.set_title('blend_img')
plt.imshow(get_magnitude_spectrum(blend_img), cmap=plt.cm.gray)

fig = plt.figure()

a = fig.add_subplot(2,2,1)
a.set_title('identity')
plt.imshow(get_magnitude_spectrum(identity_filter), cmap=plt.cm.gray)

a = fig.add_subplot(2,2,2)
a.set_title('edge')
plt.imshow(get_magnitude_spectrum(edge_filter), cmap=plt.cm.gray)

a = fig.add_subplot(2,2,3)
a.set_title('gauss')
plt.imshow(get_magnitude_spectrum(sobel_x_filter), cmap=plt.cm.gray)

#misc.imsave("qa_edge.png", get_magnitude_spectrum(edge_filter))
#misc.imsave("qa_sobel_x.png", get_magnitude_spectrum(sobel_x_filter))


# Show figures
plt.show()