using UnityEngine;
using UnityEditor;
using UnityEditorInternal;
using UnityEngine.UIElements;
// need this for dictionary
using System.Collections.Generic;


// PrefabReplaceEditor
//      class extends EditorWindow
//      creates a custom editor window for replacing any number of selected GameObject
//      to replace with a selected Prefab.
//      Found in Unity menu item EditorTools / Prefab Replace Editor Window

public class PrefabReplaceEditor : EditorWindow
{
    // create a warning message for if there are no gameObjects with the needed script in the scene
    private const string warningMsg = "No GameObjects with ReplacableObjectScript component in the scene";
    // create a warningRect to store warning message
    //  Rect constructor is (x val measured from, y val measured from, width, height)
    private static Rect warningRect = new Rect(0, 50f, 350f, 300f); 

    // create references to a SerializedObject and a ReorderableList to create
    //  functionality of adding and removing gameObjects we want to replace in the editor window
    private SerializedObject serializedObj = null;
    private ReorderableList reordList = null;

    // create a reference to my ReplacableObjectScripts class to create the array of serializedObjs
    private ReplacableObjectScript scriptObjList;
    // create an array to store the all gameObjects with ReplacableObjectScipts components
    private List<GameObject> allScriptedObjsList = new List<GameObject>();

    // variable for the selected prefab storing
    private GameObject prefab; 

    // dicts to organize the objects (and new prefabs) referenced objects and scripts during replacements 
    private Dictionary<GameObject, GameObject> objRefDict = new Dictionary<GameObject, GameObject>();
    private Dictionary<ReplacableObjectScript, ReplacableObjectScript> scriptRefDict = new Dictionary<ReplacableObjectScript, ReplacableObjectScript>();


    // ShowWindow
    //      creates a PrefabReplaceEditor window and sets a window title through GUIContent
    //  params:
    //      none
    //  returns:
    //      none
    // create a menu item to place our editor window in the menu under EditorTools
    [MenuItem("EditorTools/Prefab Replace Editor Window")]
    public static void ShowWindow()
    {
        PrefabReplaceEditor editorWin = GetWindow<PrefabReplaceEditor>();
        // create a dockable tab
        editorWin.titleContent = new GUIContent("Prefab Replace Editor Window");
    }

    // OnEnable
    //      function called when our object is loaded
    //      create the serialized object and reorderable list and draw them on the window
    //  params:
    //      none
    //  returns:
    //      none
    public void OnEnable()
    {
        // find the objects in the scene with our ReplacableObjectScript script component attached
        //  and add them to our ReplacableObjectScript list scriptObjList
        scriptObjList = FindObjectOfType<ReplacableObjectScript>();

        // check if any objects with the script were found
        if (scriptObjList != null)
        {
            // objects with scripts were found
            // lets create a serialized object of the list
            serializedObj = new SerializedObject(scriptObjList);

            // initialize the reorderable list using that serialized object
            //  params are (SerializedObject serializedObject, SerializedProperty elements, bool draggable, bool displayHeader, 
            //              bool displayAddButton, bool displayRemoveButton)
            //      our SerializedProperty is our GameObject array, called GOList, in the ReplacableObjectScript script
            reordList = new ReorderableList(serializedObj, serializedObj.FindProperty("GOList"), true, true, true, true);

            // draw the reorderable list - this list has a header and the ability to add or remove elements
            //  the header is drawn through drawHeaderCallback which takes a Rect param
            //      the => lambda operator is just to simplify calling the drawHeaderCallback function
            //      rect acts as the method parameter, and the curly brackets following => is the function definition
            //      where in this case we create a labelField in our editor GUI the size of rect that reads "Game Objects to Replace"
            reordList.drawHeaderCallback = (rect) => 
            {
                EditorGUI.LabelField(rect, "Game Objects to Replace:");
            };
            //  the elements in the reorderable list are drawn through drawElementCallback which takes four params:
            //  a Rect rect, and int index, a bool isActive, and a bool isFocused.
            //      again, => lambda is calling the drawElementCallback function with needed params, curly brackets
            //      set our rect height to be the height of a line in our GUI editor, make the label for each game object,
            //      and create a PropertyField to display our list of objects 
            reordList.drawElementCallback = (Rect rect, int index, bool isActive, bool isFocused) =>
            {
                rect.height = EditorGUIUtility.singleLineHeight;
                GUIContent objLabel = new GUIContent($"GameObject {index}");
                // PropertyField params: Rect position, SerializedProperty property, GUIContent label, bool includeChildern = false
                EditorGUI.PropertyField(rect, reordList.serializedProperty.GetArrayElementAtIndex(index), objLabel);
            };
        }


        // create a temporary list to store all ReplacableObjectScript components in the scene
        ReplacableObjectScript[] tempList = FindObjectsOfType<ReplacableObjectScript>();
        // get the length of the list so we can loop through
        int listLen = tempList.Length;
        for (int index = 0; index < listLen; index++)
        {
            // extract the gameObjects from the ReplacableObjectScript components 
            // add those gameObjects to our allScriptedObjsList
            allScriptedObjsList.Add(tempList[index].gameObject);
        }
    }


    // OnGUI
    //      creates the user interface for the editor window
    //  params:
    //      none
    //  returns:
    //      none
    public void OnGUI()
    {
        // GUILayout used to add the needed UI elements
        // title Label made with bigger font
        GUILayout.Space(15f);
        GUI.skin.label.fontSize = 18;
        GUILayout.Label("Replace GameObjects with A PreFab");
        GUILayout.Space(15f);
        // with more time, I would have made a style sheet to better format
        //  the layout, sizing, wrapping, etc of the Editor Window

        // check if there are any objects with the ReplacableObjectScript scripts component in our list
        //  (these were found in the OnEnable functions)
        if (serializedObj == null)
        {
            // there were no objects found with the script, tell the user and exit
            EditorGUI.HelpBox(warningRect, warningMsg, MessageType.Warning);
            return;
        }
        
        else if (serializedObj != null)
        {
            // there are objects with our ReplacableObjectScript scripts component
            //  update our serialized object list
            serializedObj.Update();
            // display the UI to add objects to our list in the GUI
            reordList.DoLayoutList();
            serializedObj.ApplyModifiedProperties();
        }

        // make the UI to input the prefab 
        GUILayout.Space(10f);
        GUI.skin.label.fontSize = 12;
        GUILayout.Label("Replacement Prefab:");
        // EditorGUILayout params are (Object obj, Type objType, bool allowSceneObjects)
        //  this is an Object method so we have to cast it to GameObject in order to assign to a prefab
        prefab = (GameObject)EditorGUILayout.ObjectField(prefab, typeof(GameObject), true);
        GUILayout.Space(20f);

        // make the button UI to replace the GameObjects with the prefab
        // the button should call our Replace function then close the editor window
        if (GUILayout.Button("Replace GameObjects with Prefabs"))
        {
            Replace();
            this.Close();
        }
    }

    // Replace
    //      called when the GUI button is pressed to replace selected GameObjects
    //      with the selected prefab.
    //      The prefab replacement should maintain the same translation, rotation, and scale 
    //      as the object it was swapped with. Bonus, the prefabs should be transferred the 
    //      values of reference from the script and validate for missing references.
    //      Calls two helper functions, ReplaceObject and ReplaceReferences to do this.
    //  params:
    //      none
    //  returns:
    //      none
    public void Replace()
    {   
        // check if the prefab is empty
        if (prefab == null)
        {
            // no prefab selected, so just return
            return;
        }

        // create variables to enhance readability and save memory space from
        //  recreating varaibles and increased GetComponent calls.
        GameObject newPrefab = null;
        ReplacableObjectScript newScript = null;
        ReplacableObjectScript oldScript = null;
        
        ReplaceObject(newPrefab, newScript, oldScript);

        ReplaceReferences(newPrefab, newScript, oldScript);

        CheckExistingGOs(oldScript);

        // clear the dictionaries just in case
        scriptRefDict.Clear();
        objRefDict.Clear();
    }


    // ReplaceObject
    //      helper function called by Replace()
    //      replaces an existing GameObject specified by the user with a Prefab specified by the user
    //      the Prefab will have the same position, rotation and scale as the replaced GameObject.
    //      We also update our dictionaries to keep track of what GameObject is replaced with what Prefab
    //      and what GameObject script is replaced with what Prefab script.
    //  params:
    //      newPrefab: a GameObject representing the new prefab to be instantiated
    //      newScript: a ReplacableObjectScript representing the new script to be added as a component to the 
    //                  new prefab
    //      oldScript: a ReplacableObjectScript representing the script component of the GameObject being replaced
    //  returns:
    //      none
    public void ReplaceObject(GameObject newPrefab, ReplacableObjectScript newScript, ReplacableObjectScript oldScript)
    {
        // loop through the game objects in the scriptObjList with our custom GetList function
        foreach (var oldObj in scriptObjList.GetList())
        {
            // check that the current gameObject is not null
            if (oldObj == null)
            {
                // if it is null, continue to the next object
                continue;
            }  
            // create a new GameObject out of the selected prefab and rename it    
            newPrefab = (GameObject)PrefabUtility.InstantiatePrefab(prefab);
            newPrefab.name = "Prefab_" + oldObj.name;
            // set the new prefabs position, rotation, and scale to match the existing gameObject
            newPrefab.transform.position = oldObj.transform.position;
            newPrefab.transform.rotation = oldObj.transform.rotation;
            newPrefab.transform.localScale = oldObj.transform.localScale;

            // add the oldObj and new prefab to the reference dictionary to fix object references
            objRefDict.Add(oldObj, newPrefab);

            // create a script and add it to the prefab
            newScript = newPrefab.AddComponent<ReplacableObjectScript>();
            newScript.referenceObject = null;
            newScript.referenceScript = null; 

            // the reference script attached for oldObj is found with oldObj.GetComponent<ReplacableObjectScript>().referenceScript
            // we want to store oldObj script as a key with newObj script as the value
            oldScript = oldObj.GetComponent<ReplacableObjectScript>();
            if (oldScript != null)
            {
                scriptRefDict.Add(oldScript, newScript);
            }
        }
    }


    // ReplaceReferences
    //      helper function called by Replace()
    //      updates a Prefab's script component with the referenced object and script from the GameObject
    //      the prefab replaced. These references object and script may be new prefabs and prefab scripts. 
    //      These are found through our dictionaries of old object, new prefab key value pairs, and 
    //      old object script, new prefab script key value pairs.
    //  params:
    //      newPrefab: a GameObject representing the new prefab to be instantiated
    //      newScript: a ReplacableObjectScript representing the new script to be added as a component to the 
    //                  new prefab
    //      oldScript: a ReplacableObjectScript representing the script component of the GameObject being replaced
    //  returns:
    //      none
    public void ReplaceReferences(GameObject newPrefab, ReplacableObjectScript newScript, ReplacableObjectScript oldScript)
    {
        foreach (var oldObj in scriptObjList.GetList())
        {
            // check that the current gameObject is not null
            if (oldObj == null)
            {
                // it is null, so conntinue to next object
                continue;
            }

            // objRefDict[oldObj] gives the new prefab in its place
            newPrefab = objRefDict[oldObj];
            // the newScript is the script component of that newPrefab
            newScript = newPrefab.GetComponent<ReplacableObjectScript>();
            
            // store the script component of the oldObj in oldScript
            oldScript = oldObj.GetComponent<ReplacableObjectScript>();
            // check if the oldObj has a referenced script in its oldScript component
            if (oldScript.referenceScript != null)
            {
                // check if the referenced script is in our dictionary
                if (scriptRefDict.ContainsKey(oldScript.referenceScript))
                {
                    // it is, so right now are looking at oldObj's script component's REFERENCED script, which is in our dictionary
                    //      which means that referenced scirpt's associated object has a new prefab that it was replaced with.
                    //  set our prefab's script component's Referenced script to be the old script's referenced script's prefab
                    //      this prefab's newScript is the value to the oldScripts key in our scriptRefDict
                    newScript.referenceScript = scriptRefDict[oldScript.referenceScript];
                } 
                else 
                {
                    // the referenced script is not in our dictionary, so we can just set our newScript to be
                    //      this oldScript's referencedScript (most likely another GameObject in the scene's script that
                    //      was not replaced)
                    newScript.referenceScript = oldScript.referenceScript;   
                }
            }

            // check if the oldObj has a referenced object in its oldScript component
            if (oldScript.referenceObject != null)
            {
                // check if the referenced object is in our dictionary
                if (objRefDict.ContainsKey(oldScript.referenceObject))
                {
                    // the oldScript is referencing an object in our object directory, which means that referenced object
                    //      has a new prefab it was replaced with. Set our prefab's referenced object to be the old scripts
                    //      referenced object's replaced prefab.
                    newScript.referenceObject = objRefDict[oldScript.referenceObject];
                }
                else
                {
                    // the oldScript is not referencing a object that was replaced with a prefab - maybe it references something else 
                    //      in the scene unaffected or it is null, either way we can just set the object reference
                    newScript.referenceObject = oldScript.referenceObject;
                }
            }
            // now that the replacement is complete, we can set the oldObj as inactive
            oldObj.SetActive(false);            
        }
    }


    // CheckExistingGOs
    //      helper function called by Replace()
    //      loops through the list of gameObjects with the ReplacableObjectScript to look for gameObjects
    //      in the scene that were NOT replaced with Prefabs. Ensure these gameObjects have updated 
    //      references in their ReplacableObjectScript component.
    //  params:
    //      oldScript: a ReplacableObjectScript representing the script component of the GameObject in the scene
    //  returns:
    //      none
    public void CheckExistingGOs(ReplacableObjectScript oldScript)
    {
        foreach (var obj in allScriptedObjsList)
        {
            // check if the object is still active
            if (obj.activeSelf)
            {
                // get the script component from the object
                oldScript = obj.GetComponent<ReplacableObjectScript>();
                // check if this object's reference object in their oldScript has been replaced by a prefab
                if (oldScript.referenceObject != null)
                {
                    // check if the referenced object is in our dictionary
                    if (objRefDict.ContainsKey(oldScript.referenceObject))
                    {
                        // the oldScript is referencing an object in our object directory, which means that referenced object
                        //      has a new prefab it was replaced with. Set our prefab's referenced object to be the old scripts
                        //      referenced object's replaced prefab.
                        oldScript.referenceObject = objRefDict[oldScript.referenceObject];
                    }
                    // if not, then we don't have to worry about replacing this object
                }
                // check if this object's referenced script in their oldScript has been replaced by a prefab
                if (oldScript.referenceScript != null)
                {
                    // check if the referenced script is in our dictionary
                    if (scriptRefDict.ContainsKey(oldScript.referenceScript))
                    {
                        // it is, so right now are looking at oldObj's script component's REFERENCED script, which is in our dictionary
                        //      which means that referenced scirpt's associated object has a new prefab that it was replaced with.
                        //  set our prefab's script component's Referenced script to be the old script's referenced script's prefab
                        //      this prefab's newScript is the value to the oldScripts key in our scriptRefDict
                        oldScript.referenceScript = scriptRefDict[oldScript.referenceScript];
                    } 
                    // if not, then we dont have to worry about replacing the script
                }
            }
        }

    }
}
