# -*- coding: utf-8 -*-
"""
Homework 3 problem 2
Mary Montgomery
"""

import cv2
import numpy as np
import sys


'''
check_if_rotated
check's to make sure all images are horizontal
rotates the image if it is vertical instead of horizontal
'''
def check_if_rotated(img):
    row, col, color = img.shape
    rotated = False
    if row > col:
        img = np.rot90(img)
        rotated = True
    return img, rotated

'''
calcEnergy
Computes the sum of the absolute values of the x and y derivatives of the image produced by
the OpenCV function Sobel
'''
def calcEnergy(img):
    gray_img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    sobelx = cv2.Sobel(np.float32(gray_img), cv2.CV_64F, 1, 0, ksize=3)
    sobely = cv2.Sobel(np.float32(gray_img), cv2.CV_64F, 0, 1, ksize=3)
    energy = np.add(np.abs(sobelx), np.abs(sobely))
    return energy

'''
calcWeight
using the energy matrix created by calcEnergy with the Sobel function,
we assign weights to each index of the image based on this "energy".
We assign columns 0 and N − 1 to be infinity since we do not want to delete the edges.
We go through the matrix like so:
-go through starting with the first row (this is just a row of energies)
-set the 0th and last indicies to be infinity
-get all indicies between the first and last (exclusive)
-go to the row below, for each index you add the min energy from the 
 row above (just contained to the touching indecies) (we use a stacking method for comparison)
 say this is the original energy matrix:
 ____________________________________
 |__5_|__8_|_12_|__2_|_22_|__8_|__9_|
 |__3_|_12_|__7_|_21_|_14_|__4_|_10_|
 |_11_|__3_|__9_|_13_|__6_|_32_|_23_|
 
 using this example, we then add the min touching index to the index below it, like so:
 ____________________________________
 |__5_|__8_|_12_|__2_|_22_|__8_|__9_|
 |__8_|_17_|__9_|_23_|_16_|_12_|_18_|
 the 8 above comes from the index being 3 originally, then taking the min of the indicies touching
 above (5 and 8, so 5 is the min) and adding that min to the index (3+5=8)
 the 17 above comes from the index being 12 originally, then taking the min of the touching indicies (5, 8, 12)
 in the row above and adding the min of those 3 indicies to the index (12+5 = 17)
 etc following this pattern, the following row using these newly added numbers would be:
 |_19_|_11_|_18_|_22_|_18_|_44_|_35_|
 the 19 comes from orginal index 11 adding the min of the touching above indices (8, 17) (11+8=19), etc
''' 
def calcWeight(energy):
    weight = np.copy(energy)
    weight[0][0] = np.inf
    weight[0][-1] = np.inf
    for i in range(1, len(energy)):
        #we get the row above the current row since that is what 
        #will be added on to the current row
        row = np.copy(weight[i-1,:])
        row[0] = np.inf
        row[-1] = np.inf
        is_min = np.zeros_like(row)
        #we get the staggard indices (left, right, center) so we can stack them for comparison
        #each col of the stacked array will be the comparison between one index and its two neighbors
        left = row[ :-2]
        right = row[ 2: ]
        center = row[ 1:-1 ]
        stack = np.vstack((left, right, center))
        #is_min is the array of the previous row in weight
        is_min[ 1:-1 ] = np.amin(stack, axis=0)
        is_min[0] = np.inf
        is_min[-1] = np.inf
        #the min value of the stack gets added to the index in the row below
        weight[i,:] = np.add(is_min, weight[i,:])
    return weight

'''
findSeam
just finds the seam that will be deleted
-starts at the bottom row of the image
-finds the index of the minimum value
-goes up the rows towards the top of the image,
    checking the indicies of the minimum index and its neighbors
    to find the path up the image of the seam
'''
def findSeam(weight):
    seam = []
    min_index = np.argmin(weight[(len(weight)-1), :])
    seam.append(min_index)
    for i in range(len(weight)-2, -1, -1):
        row = weight[i, :]
        temp_index = np.argmin(row[(min_index-1) : (min_index+2)])
        if temp_index == 0:
            min_index -= 1
        elif temp_index == 2:
            min_index += 1
        seam.append(min_index)
    return seam

'''
plotFirstSeam
plotting the seam on the image so that it shows in red
this is written to the output image file of the proper name
'''
def plotFirstSeam(img, seam, file_name):
    im = np.copy(img)
    for i in range(0,len(seam)):
        im[i][seam[len(seam)-1-i]][0] = 0
        im[i][seam[len(seam)-1-i]][1] = 0
        im[i][seam[len(seam)-1-i]][2] = 255
    output_name = file_name + "_seam.png"
    cv2.imwrite(output_name, im)      

'''
deleteSeam
deleting the indicies of the seam from the image
using a mask which creates the image in bools and sets
the seam index to be deleted to be false so when the image
is merged back together only the true indicies will remain, 
properly cutting out the seam
'''
def deleteSeam(img, seam):
    rows, cols, color = img.shape
    mask = np.ones_like(img, dtype=bool)
    for i in range(len(img)-1, -1, -1):
        index = seam[(len(seam)-1) - i]
        mask[i][index] = False
    img = img[mask]
    img = np.reshape(img, (rows, cols-1, color))
    return img


'''
output
a proper format of the output needed using all information found
through the other functions
'''
def output(row, col, seam, total_energy, seam_num, rotate):
    print("\nPoints on seam %d:" %seam_num)
    if rotate:
        print("horizontal")
        print("%d, %d" %(seam[0], 0))
        print("%d, %d" %(seam[len(seam)//2], cols//2))
        print("%d, %d" %(seam[-1], cols-1))
        print("Energy of seam %d: %.2f" %(seam_num, total_energy/cols))
    else:
        print("vertical")
        print("%d, %d" %(0, seam[-1]))
        print("%d, %d" %(row//2, seam[len(seam)//2]))
        print("%d, %d" %(row-1, seam[0]))
        print("Energy of seam %d: %.2f" %(seam_num, total_energy/row))
    


#main method
if __name__ == "__main__":
    #read in the file
    file = sys.argv[1]
    #save the file name (without the .png)
    file_name = file[:-4]
    #read in the file to load the image
    img = cv2.imread(file)
    #check if you need to rotate the image
    img, rotate = check_if_rotated(img)
    #get the shape of the image
    row, cols, color = img.shape
    #get first seam info
    #feed image into energy
    first_energy = calcEnergy(img)   
    #feed energy into weight matrix
    first_weight = calcWeight(first_energy)
    #feed weight into finding the seam
    first_seam = findSeam(first_weight)
    #plot the first seam
    plotFirstSeam(img, first_seam, file_name)
    #find the total energy for the first seam (to be used for average energy of the seam)
    first_seam_total_energy = first_weight[len(first_weight)-1][first_seam[0]]
    img = deleteSeam(img, first_seam)
    #get the second seam info
    #feed image into energy
    second_energy = calcEnergy(img)   
    #feed energy into weight matrix
    second_weight = calcWeight(second_energy)
    #feed weight into finding the seam
    second_seam = findSeam(second_weight)
    #find the total energy for the second seam (to be used for average energy of the seam)
    second_seam_total_energy = second_weight[len(second_weight)-1][second_seam[0]]
    img = deleteSeam(img, second_seam)
    #count the seams for the output
    seam_count = 1
    #take slice, calculate weight, find the seam, delete the seam, repeat
    #do this until the image is square
    #this will always be rows < cols because the image is horizontal no matter what
    #so the cols are always what is declining
    while row < cols:
        energy = calcEnergy(img)
        weight = calcWeight(energy)
        seam = findSeam(weight)
        img = deleteSeam(img, seam)
        row, cols, color = img.shape
        seam_count += 1
    #next two lines needed for the output
    last_seam = seam
    last_seam_total_energy = weight[len(weight)-1][seam[0]]
    #output is below, seam num is for labeling the seam in the output
    seam_num = 0
    output(row, cols, first_seam, first_seam_total_energy, seam_num, rotate)
    seam_num = 1
    output(row, cols, second_seam, second_seam_total_energy, seam_num, rotate)
    seam_num = seam_count
    output(row, cols, last_seam, last_seam_total_energy, seam_num, rotate)
    #if rotated at the beginning, rotate it back
    if rotate:
        img = np.rot90(img, k=3)
    #create the output file name
    output_name = file_name + "_final.png"
    #write the final image to the output file
    cv2.imwrite(output_name, img)
