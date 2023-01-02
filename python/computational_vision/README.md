"Seam Carving"
This assignment uses a seam carving "energy" technique to resize a rectangular image into a square image. 

What I did:
I start with an image, and first make sure it is horizontally formatted so I can resize it.

I then calculate the "energy" of each index in the image using the OpenCV Sobel function. The Sobel function is used to find the derivative of an image using gradient changes in the image. The function is used for edge detection, so when there is an edge, we will see a major change in gradient differentiating one object from a background, or one object from another, etc. The edges would be peaks - maximums -  in the derivatives. Since for our case we want the seams with the least "energy" we are looking for the smallest amounts of energy - meaning that the gradiant barely changes, if at all - so the smallest numbers in our Sobel derivative image matrix. These are the seams that we can remove without causing too big a change in the image.

Once that energy is calculated, we calculate the weights of each index using a method that builds row upon row, using the row above to find a minimum touching index that is added to the touching index in the row below. Here is a demo of what I mean: 
say this is the original energy matrix:
 ________________
 |  5 |  8 | 12 |
 
 |  3 | 12 |  7 |
 
 | 11 |  3 |  9 |
 
 using this example, we then add the min touching index to the index below it, like so:
 ________________
 |  5 |  8 | 12 |
 
 |  8 | 17 |  9 |

the 8 above comes from the index being 3 originally, then taking the min of the indicies touching above (5 and 8, so 5 is the min) and adding that min to the index (3+5=8)
the 17 above comes from the index being 12 originally, then taking the min of the touching indicies (5, 8, 12) in the row above and adding the min of those 3 indicies to the index (12+5 = 17)
etc following this pattern, the following row using these newly added numbers would be:

 | 19 | 11 | 18 |

the 19 comes from orginal index 11 adding the min of the touching above indices (8, 17) (11+8=19), etc.

Next, we find the seam of lowest energy by starting at the bottom of the image and looking for the minimum in that row. We follow this minimum index chain up to the top of the image. Once we have the seam found, we mask the image, set the seam indicies to be false, and remerge the image without the seam. 

We repeat this pattern of calculating the energy, weight, seam finding and seam deletion until our image is square.

Files:
p2_seam_carve is the main python file, the other images show the output of the file. The original files are ryan.jpg and whistler.png, the respective image_seam images show what the program does, showing the seams of lowest energy that will first be removed, then the final images have image_final format.
