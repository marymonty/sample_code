#combinedCrowds.py

import maya.cmds as cmds
import functools
import random

# userSelectUI
#    creates the first UI the user inteacts with
#    prompts for the way the user wants to create a crowd: by number, body type, or both
def userSelectUI():
    # we can uniquely identify each window to make sure only
    # one instance is open at any given time
    # so create and identify our window, then check if it already exists
    windowID = "myWindowID"
    if cmds.window( windowID, exists=True ):
        #it does already exist, so delete it
        cmds.deleteUI( windowID )
        
    #create the window  
    cmds.window( windowID, title="How do you want to Create a Crowd?", sizeable=False, resizeToFitChildren=True, backgroundColor= [0.8, 0.78, 1.0] )
    
    # create a columnLayout to determnine elements in UI layout
    cmds.columnLayout( adjustableColumn = True )
    
    
    # create crowd by body type only
    userSelection = cmds.radioButtonGrp( numberOfRadioButtons=3, label="Create a crowd by:", labelArray3=["Number", "Body Type", "Number and Body Type"], height=50)

    # our final row has our buttons in the 0th and 1st cols
    # when the button is pressed, we use the functools partial function
    # which wraps the callback function along with its arguments
    # but delays its execution until the apply button is pressed 
    cmds.button( label="Apply", backgroundColor= [0.7, 0.68, 1.0], height=30, command=functools.partial( returnUserCrowdType,
                                                                                            userSelection, windowID ) )
    
    #create a cancelCallback function
    # takes unspecified number of args, denoted by *
    def cancelCallback( *pArgs):
        if cmds.window( windowID, exists=True ):
            cmds.deleteUI( windowID ) 
            
    cmds.button( label="Close Window", backgroundColor=[0.7, 0.68, 1.0], height=30, command=cancelCallback )
    
    #display the window with showWindow function
    cmds.showWindow()
    
    
#createCrowdUI
# need to create the correct UI as requested by the user:
#   1 = create crowd by number only
#   2 = create crowd by body type only
#   3 = create crowd by number and body type
# takes the user UI selection as param and creates the UI window for user input
def createCrowdUI( pUserSelection ):
    
    # we can uniquely identify each window to make sure only
    # one instance is open at any given time
    # so create and identify our window, then check if it already exists
    windowID = "myWindowID"
    if cmds.window( windowID, exists=True ):
        #it does already exist, so delete it
        cmds.deleteUI( windowID )
    
    buttonBGColor = [0, 0, 0]    
    # create a window based on the user selection
    if ( pUserSelection == 1 ):
        # create the window
        cmds.window( windowID, title="Create a Crowd", sizeable=False, resizeToFitChildren=True, backgroundColor=[1.0, 1.0, 0.8] )

        # create a columnLayout to determnine elements in UI layout
        cmds.columnLayout( adjustableColumn = True )

        # create crowd by number only
        cmds.text( label="Number of crowd members:", height=50 )
        # numPeopleField lets user provide number of crowd members, defaults to 25
        numPeopleField = cmds.intField( value=25, height=35 )
        cmds.separator( h=20 )
        
        buttonBGColor = [1.0, 1.0, 0.6] 

        # our final row has our buttons in the 0th and 1st cols
        # when the button is pressed, we use the functools partial function
        # which wraps the callback function along with its arguments
        # but delays its execution until the apply button is pressed 
        cmds.button( label="Apply", backgroundColor=buttonBGColor, height=30, command=functools.partial( makeCrowdByNumber,
                                                                                            numPeopleField ) )
    
    elif ( pUserSelection == 2 ):
        # create the window
        cmds.window( windowID, title="Create a Crowd", sizeable=False, resizeToFitChildren=True, backgroundColor=[0.7, 0.9, 0.85] )

        # create a columnLayout to determnine elements in UI layout
        cmds.columnLayout( adjustableColumn = True )
        
        # create crowd by body type only
        headType = cmds.radioButtonGrp( numberOfRadioButtons=3, label="Head Types:", labelArray3=["Sphere", "Torus", "Cube"], height=50 )
            
        bodyType = cmds.radioButtonGrp( numberOfRadioButtons=3, label="Body Types:", labelArray3=["Prism", "Torus", "Icosahedron"], height=50 )
        
        legType = cmds.radioButtonGrp( numberOfRadioButtons=3, label="Leg Types:", labelArray3=["Cube", "Cone", "Cylinder"], height=50 )

        buttonBGColor = [0.6, 0.8, 0.75] 

        # our final row has our buttons in the 0th and 1st cols
        # when the button is pressed, we use the functools partial function
        # which wraps the callback function along with its arguments
        # but delays its execution until the apply button is pressed 
        cmds.button( label="Apply", backgroundColor=buttonBGColor, height=30, command=functools.partial( makeCrowdByBodyType,
                                                                                                headType, bodyType, legType ) )

    
    elif ( pUserSelection == 3 ):
        # create the window
        cmds.window( windowID, title="Create a Crowd", sizeable=False, resizeToFitChildren=True, backgroundColor=[0.85, 0.7, 0.95] )

        # create a columnLayout to determnine elements in UI layout
        cmds.columnLayout( adjustableColumn = True )
        
        # create crowd by number and body type
        headType = cmds.radioButtonGrp( numberOfRadioButtons=3, label="Head Types:", labelArray3=["Sphere", "Torus", "Cube"], height=50 )
            
        bodyType = cmds.radioButtonGrp( numberOfRadioButtons=3, label="Body Types:", labelArray3=["Prism", "Torus", "Icosahedron"], height=50 )
        
        legType = cmds.radioButtonGrp( numberOfRadioButtons=3, label="Leg Types:", labelArray3=["Cube", "Cone", "Cylinder"], height=50 )
        
        cmds.text( label="Number of this crowd member type:", height=25 )
        numMember = cmds.intField( value=1, height=30 )
        
        buttonBGColor = [0.75, 0.5, 0.85] 
        
        # our final row has our buttons in the 0th and 1st cols
        # when the button is pressed, we use the functools partial function
        # which wraps the callback function along with its arguments
        # but delays its execution until the apply button is pressed 
        cmds.button( label="Apply", backgroundColor=buttonBGColor, height=30, command=functools.partial( makeCrowdByBodyAndNumber,
                                                                headType, bodyType, legType,
                                                                numMember ) )

    cmds.button( label="Clear scene and Restart", backgroundColor = buttonBGColor, height=30, command=functools.partial( clearAndRestart ) )
   
    #create a cancelCallback function
    # takes unspecified number of args, denoted by *
    def cancelCallback( *pArgs):
        if cmds.window( windowID, exists=True ):
            cmds.deleteUI( windowID ) 
            
    cmds.button( label="Close Window", height=30, backgroundColor = buttonBGColor, command=cancelCallback )
    
    #display the window with showWindow function
    cmds.showWindow()


    
    

# returnUserCrowdType
#    closes the user selection type UI and opens up the crowd builder UI
def returnUserCrowdType(userSelection, windowID, *pArgs):
    selectedType = cmds.radioButtonGrp( userSelection, query=True, select=True )
    print("selected type: %s" %(selectedType) )
    # close the user selection UI
    if cmds.window( windowID, exists=True ):
        cmds.deleteUI( windowID )
    # call the function to open to crowd creater UI
    createCrowdUI(selectedType)
     

# makeCrowdByNumber
#     takes in the number of crowd members to create as input
#     and creates that many number of randomized crowd members 
def makeCrowdByNumber( pNumMembers, *pArgs ):
    # get the number from the user argument through query
    numMembers = cmds.intField( pNumMembers, query=True, value=True )
    
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
    
    
    for i in range(0, numMembers):
        #create an instance of one of the parts at random
        m = int(random.uniform(0, numBodyParts))
        head = headsArr[m]
        n = int(random.uniform(0, numBodyParts))
        body = bodyArr[n]
        p = int(random.uniform(0, numBodyParts))
        feet = feetArr[p]
        
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



# makeCrowdByBodyType
#     takes in the users selected head, body, and leg type
#     and creates a crowd members with those specified features
def makeCrowdByBodyType( pHeadField, pBodyField, pLegField, *pArgs ):
    # get the data out of the args by querying them
    selectedHead = cmds.radioButtonGrp( pHeadField, query=True, select=True )
    selectedBody = cmds.radioButtonGrp( pBodyField, query=True, select=True )
    selectedFeet = cmds.radioButtonGrp( pLegField, query=True, select=True )
    
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

    #create the user selected body parts
    head = headsArr[selectedHead-1]
    body = bodyArr[selectedBody-1]
    feet = feetArr[selectedFeet-1]
    
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


# makeCrowdByBodyAndNumber
#     takes in the user selected head, body, and leg types as well as the 
#     number of crowd members and creates that many number of specified crowd members 
def makeCrowdByBodyAndNumber( pHeadField, pBodyField, pLegField, pNumMember, *pArgs ):
    # get the data out of the args by querying them
    selectedHead = cmds.radioButtonGrp( pHeadField, query=True, select=True )
    selectedBody = cmds.radioButtonGrp( pBodyField, query=True, select=True )
    selectedFeet = cmds.radioButtonGrp( pLegField, query=True, select=True )
    numMembers = cmds.intField( pNumMember, query=True, value=True )
    
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
    
    
    for i in range(0, numMembers):
        #create an instance of one of the parts at random
        #m = int(random.uniform(0, numBodyParts))
        head = headsArr[selectedHead-1]
        #n = int(random.uniform(0, numBodyParts))
        body = bodyArr[selectedBody-1]
        #p = int(random.uniform(0, numBodyParts))
        feet = feetArr[selectedFeet-1]
        
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

    
# clearAndRestart
#    delete all the DAG objects in the scene and repropt the first UI
#    that gives the user to option of which type of crowd to create
def clearAndRestart( *pArgs ):
    # select all DAG objects to delete from the scene
    cmds.select( allDagObjects = True )
    # delete those selected objects
    cmds.delete()
    # call the userSelectUI to restart the process
    userSelectUI()



# start the program by calling the userSelectUI to prompt the user
userSelectUI()
