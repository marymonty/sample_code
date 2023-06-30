#crowdByBodyType.py

import maya.cmds as cmds
import functools
import random


#createUI
# takes the name of the window, and a callback function as params
# creates the UI window for user input
def createUI( pWindowTitle, pApplyCallback ):
    
    # we can uniquely identify each window to make sure only
    # one instance is open at any given time
    # so create and identify our window, then check if it already exists
    windowID = "myWindowID"
    if cmds.window( windowID, exists=True ):
        #it does already exist, so delete it
        cmds.deleteUI( windowID )
        
    #create the window  
    cmds.window( windowID, title=pWindowTitle, sizeable=False, resizeToFitChildren=True )
    
    # create a rowColumnLayout to determnine elements in UI layout
    cmds.rowColumnLayout( numberOfColumns=2, columnWidth=[ (1, 500), (2, 60) ], columnOffset=[ (1, "left", 5) ] )
    
    #using layout, add text, input fields, button, etc
    #cmds.text( label="Head Type:" )
    headType = cmds.radioButtonGrp( numberOfRadioButtons=3, label="Head Types:", labelArray3=["Sphere", "Torus", "Cube"])
    cmds.separator( h=20 )
        
    bodyType = cmds.radioButtonGrp( numberOfRadioButtons=3, label="Body Types:", labelArray3=["Prism", "Torus", "Icosahedron"])
    cmds.separator( h=20 )
    
    legType = cmds.radioButtonGrp( numberOfRadioButtons=3, label="Leg Types:", labelArray3=["Cube", "Cone", "Cylinder"])
    cmds.separator( h=20 )
    
    # our final row has our buttons in the 0th and 1st cols
    # when the button is pressed, we use the functools partial function
    # which wraps the callback function along with its arguments
    # but delays its execution until the apply button is pressed 
    cmds.button( label="Apply", command=functools.partial( pApplyCallback,
                                                            headType, bodyType, legType ) )
    
    #create a cancelCallback function
    # takes unspecified number of args, denoted by *
    def cancelCallback( *pArgs):
        if cmds.window( windowID, exists=True ):
            cmds.deleteUI( windowID ) 
            
    cmds.button( label="Cancel", command=cancelCallback )
    
    #display the window with showWindow function
    cmds.showWindow()


#createCrowd
# takes in the number of crowd members to create as input
# and creates that many number of randomized crowd members 
def createCrowd( pHeadType, pBodyType, pLegType ):
    
    # randomize using new seeds
    seed = random.uniform( 0, 100 )
    random.seed( seed )
    
    #create a var for the number of body parts
    numBodyParts = 3
    
    ##############      HEADS       ################
    #create a sphere, torus, and cube for different head shapes
    sphereHead = cmds.polySphere( r=0.75, name="sphereHead#" )
    torusHead = cmds.polyTorus( r=0.5, sr=0.25, name="torusHead#" )
    cubeHead = cmds.polyCube( sx=5, sy=5, sz=5, name="cubeHead#" ) 
    #we want the original torus to be rotated 90 degrees on the x axis
    cmds.rotate( 90, 0, 0, torusHead )
    #create the heads array
    headsArr = [ sphereHead, torusHead, cubeHead ]
    
    ##############      BODIES       ################
    #create a sphere, torus, and cube for different head shapes
    prismBody = cmds.polyPrism( ns=3, w=3, name="prismBody#" )
    torusBody = cmds.polyTorus( r=0.75, sr=0.5, name="torusBody#" )
    diceBody = cmds.polyPlatonicSolid( r=0.75, l=0.9, name="diceBody#" ) 
    #we want the original torus to be rotated 90 degrees on the x axis
    cmds.rotate( 90, 0, 0, torusBody )
    #create the body array
    bodyArr = [ prismBody, torusBody, diceBody ]
    
    ##############      FEET       ################
    #create a sphere, torus, and cube for different head shapes
    cubeFeet = cmds.polyCube( d=0.75, h=1.0, w=0.75, name="cubeFeet#" )
    coneFeet = cmds.polyCone( r=0.5, name="coneFeet#" )
    cylFeet = cmds.polyCylinder( r=0.5, name="cylFeet#" ) 
    #create the feet array
    feetArr = [ cubeFeet, coneFeet, cylFeet ]
    
    #group by body
    bodyGroup = cmds.group( empty=True, name="body_group#" )
    
    
    #for i in range(0, 25):
    #create an instance of one of the parts at random
    #m = int(random.uniform(0, numBodyParts))
    head = headsArr[pHeadType]
    #n = int(random.uniform(0, numBodyParts))
    body = bodyArr[pBodyType]
    #p = int(random.uniform(0, numBodyParts))
    feet = feetArr[pLegType]
    
    #randomly move the parts about the x and z axis only
    xMove = random.uniform(-20, 20)
    zMove = random.uniform(-20, 20)
    cmds.move( xMove, 4, zMove, head )
    cmds.move( xMove, 2.5, zMove, body )
    cmds.move( xMove, 1, zMove, feet )
    
    #randomly rotate the parts about the y axis only
    yRot = random.uniform(0, 360)
    cmds.rotate( 0, yRot, 0, head, relative=True )
    cmds.rotate( 0, yRot, 0, body, relative=True )
    cmds.rotate( 0, yRot, 0, feet, relative=True )
    
    instanceHead = cmds.instance( head[0], name=head[0] + '_instance#' )
    instanceBody = cmds.instance( body[0], name=body[0] + '_instance#' )
    instanceFeet = cmds.instance( feet[0], name=feet[0] + '_insatnce#' )
    #group our bodies together to create a person
    personGroup = cmds.group( instanceHead, instanceBody, instanceFeet, n="person#" )
    #group all people together in a larger bodies group
    cmds.parent( personGroup, bodyGroup )
    
        
    for j in range(0, numBodyParts):
        #hide the original body parts
        cmds.hide( headsArr[j] )
        cmds.hide( bodyArr[j] )
        cmds.hide( feetArr[j] )
    
    #set the pivot points to the center of the objects bounding box
    cmds.xform( bodyGroup, centerPivots=True )


  
# test our interface with an apply callback funtion which prints
# a simple message    
def applyCallback( pHeadField, pBodyField, pLegField, *pArgs ):
    print( "Apply button pressed" )
    
    #get the input by querying the intFields
    selectedHead = cmds.radioButtonGrp( pHeadField, query=True, select=True )
    print("selected head: %s" %(selectedHead) )
    selectedBody = cmds.radioButtonGrp( pBodyField, query=True, select=True )
    print("selected body: %s" %(selectedBody) )
    selectedFeet = cmds.radioButtonGrp( pLegField, query=True, select=True )
    print("selected feet: %s" %(selectedFeet) )
    # create the crowd
    createCrowd( selectedHead-1, selectedBody-1, selectedFeet-1 )
    
 
#call our function
createUI( "Crowd Number", applyCallback )  
    
     
        