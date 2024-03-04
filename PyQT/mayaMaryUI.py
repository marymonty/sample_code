'''
    mayaMaryUI
        creates a UI in maya for a user to create a shape, change the scale of the shape,
        change the color of the shape, and change the name of the shape. 
        Attests to my general python knowledge, as well as PySide, PyQT, and maya Python 
        commands.
'''

# imports needed to connect this code to maya commands
import maya.cmds as cmds
import maya.mel as mel
from maya import OpenMayaUI as omui
from shiboken2 import wrapInstance
# need PySide to utilize PyQT functionality
from PySide2 import QtUiTools, QtCore, QtGui, QtWidgets
# needed to pass arguments during signal function calls
from functools import partial 
import sys


# python dictionary of colors:
#   color name : rgb value 
colorsDict = {
    "grey": (0.5, 0.5, 0.5),
    "red": (1,0,0),
    "yellow": (1, 1, 0),
    "green": (0,1,0),
    "blue": (0,0,1)
}

# mayaMaryUI
#   custom subclass extends QWidget
#   creates my custom UI widget (our main window)
class mayaMaryUI(QtWidgets.QWidget):
    # Create a default tool window.
    window = None
    
    # __init__
    #   extend the subclass to add content to the window using init
    #   passes args to the init of the super parent class
    def __init__(self, parent = None):
        # initialize class
        super(mayaMaryUI, self).__init__(parent = parent)
        # set standard flags
        self.setWindowFlags(QtCore.Qt.Window)
        # make the widget as QMainWindow to create our window
        self.widget = QtWidgets.QMainWindow()
        # set parent adds our widget to the window
        self.widget.setParent(self)
        # set initial window size
        width = 500
        height = 300
        self.resize(width, height)
        self.setMinimumSize(width, height)   

        # make UI layouts to add buttons to
        # QHBoxLayout creates a horizontal layout, QVBoxLayout creates vertical
        layoutHorizontalRadioButtons = QtWidgets.QHBoxLayout()
        layoutHorizontalNameInput = QtWidgets.QHBoxLayout()
        layout = QtWidgets.QVBoxLayout()

        # create UI elements using PyQT widgets
        self.btn_close = QtWidgets.QPushButton("close") 
        self.label_shapes = QtWidgets.QLabel("Make a ... ")
        self.btn_makeSphere = QtWidgets.QRadioButton("Sphere")
        self.btn_makeCube = QtWidgets.QRadioButton("Cube")
        self.btn_makeTorus = QtWidgets.QRadioButton("Torus")
        self.label_slider = QtWidgets.QLabel("Change selected shape scale: 1")
        self.slider_shapeScale = QtWidgets.QSlider()
        self.label_colors = QtWidgets.QLabel("Change selected shape color:")
        self.comboBox_colors = QtWidgets.QComboBox()
        self.label_name = QtWidgets.QLabel("Change selected shape name:")
        self.lineEdit_name = QtWidgets.QLineEdit("myAppShape")
        self.btn_deleteShape = QtWidgets.QPushButton("Delete Shape")

        # connect signals to slots
        #   the signals are the triggered events (like clicked, or textChanged)
        #   the slots are my custom functions that are called (like self.makeSphere, or self.changeColor)
        self.btn_close.clicked.connect(self.close)
        self.btn_makeSphere.clicked.connect(self.makeSphere)
        self.btn_makeCube.clicked.connect(self.makeCube)
        self.btn_makeTorus.clicked.connect(self.makeTorus)
        # set the orientation of the slider to horizontal then construct boundaries for the slider
        self.slider_shapeScale.setOrientation(QtCore.Qt.Orientation.Horizontal)
        self.slider_shapeScale.setMinimum(1)
        self.slider_shapeScale.setMaximum(10)
        self.slider_shapeScale.valueChanged.connect(self.getSliderValue)
        # add values to the combo box from our colorsDict
        for colorName in colorsDict:
            self.comboBox_colors.addItem(colorName)
        self.comboBox_colors.currentIndexChanged.connect(self.changeColor)
        self.lineEdit_name.textChanged.connect(self.changeName)
        self.btn_deleteShape.clicked.connect(self.deleteShape)

        # add UI elements to their respective layouts 
        layout.addWidget(self.label_shapes)
        # add radio shape buttons to their own horiztonal layout  
        layoutHorizontalRadioButtons.addWidget(self.btn_makeSphere)
        layoutHorizontalRadioButtons.addWidget(self.btn_makeCube)
        layoutHorizontalRadioButtons.addWidget(self.btn_makeTorus)
        # add radio button layout to main layout
        layout.addLayout( layoutHorizontalRadioButtons )
        layout.addWidget(self.label_slider)
        layout.addWidget(self.slider_shapeScale)
        layout.addWidget(self.label_colors)
        layout.addWidget(self.comboBox_colors)
        # add name changing label and input text box to their own horizontal layout
        layoutHorizontalNameInput.addWidget(self.label_name)
        layoutHorizontalNameInput.addWidget(self.lineEdit_name)
        # add the name layout to the main layout
        layout.addLayout(layoutHorizontalNameInput)
        layout.addWidget(self.btn_deleteShape)
        layout.addWidget(self.btn_close)
        
        # make a container to hold the layout and set the layout
        container = QtWidgets.QWidget()
        container.setLayout(layout)

        # add the container to self
        self.widget.setCentralWidget(container)



    # resizeEvent
    #   triggered on automatically generated resize event to
    #   resize the window
    #   params:
    #       self: the app window
    #       event: the resize event
    #   returns:
    #       none
    def resizeEvent(self, event):
        self.widget.resize(self.width(), self.height())
        

    # makeSphere
    #   makes a sphere in maya then check scale, color, name to update
    #   params:
    #       self: the app window
    #   returns:
    #       none
    def makeSphere(self):
        print("making a sphere")
        # cmds is calling maya.cmds to give us that cross functionality 
        #   with the python maya script editor. All the commands used after
        #   cmds come from maya's python documentation
        cmds.polySphere()
        self.getSliderValue()
        self.changeColor()
        self.changeName()


    # makeCube
    #   makes a cube in maya then check scale, color, name to update
    #   params:
    #       self: the app window
    #   returns:
    #       none
    def makeCube(self):
        print("making a cube")
        cmds.polyCube()
        self.getSliderValue()
        self.changeColor()
        self.changeName()

    # makeTorus
    #   makes a torus in maya then check scale, color, name to update
    #   params:
    #       self: the app window
    #   returns:
    #       none
    def makeTorus(self):
        print("making a torus")
        cmds.polyTorus()
        self.getSliderValue()
        self.changeColor()
        self.changeName()

    # getSliderValue
    #   gets the slider value and changes the scale of the selected shape
    #   params:
    #       self: the app window
    #   returns:
    #       none
    def getSliderValue(self):
        value = self.slider_shapeScale.value()
        print("slider value: %f" % value)
        cmds.scale(value, value, value)
        self.label_slider.setText("change selected shape scale: %i" % value)

    # changeColor
    #   deletes the selected shape
    #   params:
    #       self: the app window
    #   returns:
    #       none
    def changeColor(self):
        print("changing shape color")
        myColor = self.comboBox_colors.currentText()
        selectedObj = cmds.ls(sl=True, long=True) 
        # selectedObj contains a list of all selected objects (which should only ever
        #   contain one object in our case) but we just look at the first object in this
        #   list. I am slicing the string in conjunction with the indexing because 
        #   the selected object name begins with a "|" char so I want to get rid of that
        cmds.select(selectedObj[0][1:])
        # set attribute using the key value pairs, breaking up the values to fit the setAttr double3 format
        cmds.setAttr("lambert1.color", colorsDict[myColor][0], colorsDict[myColor][1], colorsDict[myColor][2], type = "double3")

    # changeName
    #   changes the name of the selected shape
    #   params:
    #       self: the app window
    #   returns:
    #       none
    def changeName(self):
        print("changing shape name")
        newName = self.lineEdit_name.text()
        selectedObj = cmds.ls(sl=True, long=True) 
        selectedObj = selectedObj[0][1:]
        cmds.rename(selectedObj, newName)

    # deleteShape
    #   deletes the selected shape
    #   params:
    #       self: the app window
    #   returns:
    #       none
    def deleteShape(self):
        print("deleting shape")
        cmds.delete()


# openWindow
#   checks if a window instance is already open, closes it if so
#   calls necessary maya functionality to create and attach a window in maya
#   gives the window a unique ID and a title
#   params:
#       none
#   returns:
#       none
def openWindow():
    # Maya uses QApplications so this should always return True
    if QtWidgets.QApplication.instance():
        # search through all QApplication windows for our specific window ID
        for win in (QtWidgets.QApplication.allWindows()):
            # if we find our window still open, destroy it since we are creating a new one
            if 'myWindowID' in win.objectName():
                win.destroy()

    # pointer to the main window needed in the wrapInstance call
    mayaMainWindowPtr = omui.MQtUtil.mainWindow()
    mayaMainWindow = wrapInstance(int(mayaMainWindowPtr), QtWidgets.QWidget)
    # create a window
    mayaMaryUI.window = mayaMaryUI(parent = mayaMainWindow)
    # set the object name - every interface in maya is a QT window, so we want to 
    #   be able to easily differentiate our window by setObjectName setting an ID
    mayaMaryUI.window.setObjectName('myWindowID')
    # set the window title
    mayaMaryUI.window.setWindowTitle('PyQT UI')
    # show the window
    mayaMaryUI.window.show()


# the only call we need to make in main is to openWindow   
openWindow()
