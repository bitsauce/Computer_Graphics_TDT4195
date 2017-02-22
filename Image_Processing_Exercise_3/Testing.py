from scipy import misc
import scipy.ndimage.morphology as morph
import numpy as np
import matplotlib.pyplot as plt

#######
#  a) #
#######

img = misc.imread("q&a_a.png", 'L')

img = img > 127

# Create figure for noise reduction
fig = plt.figure()

# Plot original
a = fig.add_subplot(2,3,1)
a.set_title('Original')
plt.imshow(img, cmap=plt.cm.gray)

# Create disk structuring element
r = 15
y,x = np.ogrid[-r : r + 1, -r : r + 1]
mask = x**2+y**2 <= r**2
mask = mask.astype(int)

a = fig.add_subplot(2,3,2)
a.set_title('Disk')
plt.imshow(mask, cmap=plt.cm.gray)

a = fig.add_subplot(2,3,3)
a.set_title('Closed')
img_closing = morph.binary_closing(img, structure=mask)
plt.imshow(img_closing, cmap=plt.cm.gray)

a = fig.add_subplot(2,3,4)
a.set_title('Opening')
img_opening = morph.binary_opening(img, structure=mask)
plt.imshow(img_opening, cmap=plt.cm.gray)


a = fig.add_subplot(2,3,5)
a.set_title('Eroded')
img_eroded = morph.binary_erosion(img, structure=mask)
plt.imshow(img_eroded, cmap=plt.cm.gray)

a = fig.add_subplot(2,3,6)
a.set_title('Dilated')
img_dilated = morph.binary_dilation(img, structure=mask)
plt.imshow(img_dilated, cmap=plt.cm.gray)

def boundaryExtraction(img):
	return img - morph.binary_erosion(img, structure=np.ones((3, 3)))	

misc.imsave("q&a_a_original.png", img);
misc.imsave("q&a_a_mask.png", mask);
misc.imsave("q&a_a_closing.png", img_closing);
misc.imsave("q&a_a_opening.png", img_opening);
misc.imsave("q&a_a_eroded.png", img_eroded);
misc.imsave("q&a_a_dilated.png", img_dilated);

# Show figures
plt.show()