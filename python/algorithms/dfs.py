#create container for nodes 
#[[node, [reachable nodes]], [node, [reachable nodes]]]
#graph = []
from collections import defaultdict

graph = defaultdict(list)


def addEdge(graph, u, v):
    graph[u].append(v)
 
    # A function used by DFS
def DFSRecursive(arr, graph, v, visited):
 	# Mark the current node as visited and print it
    visited[v]= True
    if (v != 0):
        arr.append(v)
    if (graph[0] == []) :
    	del graph[0]
    if (graph[1] == []) :
    	del graph[1]
   # Recur for all the vertices adjacent to this vertex
   #graph[2] = all the child nodes of node 2
   #the for loop goes through them and sees if any are unvisited
   #if they arent visited, it will recurse through this to visit it
 	
 #   print("\ngraph: ", graph, "\n")
 #   print("\nvisited: ", visited, "\n")
    
 
    for i in graph[v]:
    	if (i <= len(visited)):
	        if visited[i] == False:
	        	DFSRecursive(arr, graph, i, visited)
 		
 
    # The function to do DFS traversal. It uses
    # recursive DFSRecursive()
def DFS(arr, graph):
	
    V = len(graph)+2  #total vertices

        # Mark all the vertices as not visited
    visited =[False]*(V)
 
        # Call the recursive helper function to print
        # DFS traversal starting from all vertices one
        # by one
    for i in range(V):
        if visited[i] == False:
            DFSRecursive(arr, graph, i, visited)


def scc(graph, arr, i):
	returnArr = []
	returnArr.append(arr[i])
	foundChild = False
	while i < len(arr)-1:
		for child in graph[arr[i]]:
			if (arr[i+1] == child):
				returnArr.append(arr[i+1])
				foundChild = True
				arr.pop(i)
		if foundChild == True:
			foundChild = False
		else:
			arr.pop(i)
			return returnArr
			break
	return returnArr



#example.txt is smaller file
#test.txt is bigger file
file = open("test.txt")
for numberLine in file:
	numberLine.strip()
	nodes = numberLine.split(" ")
	nodes[1] = nodes[1][:-1]	#remove new line character
	nodes[0] = int(nodes[0])
	nodes[1] = int(nodes[1])
	temp = nodes[0]				#next three lines are reversing the order
	nodes[0] = nodes[1]
	nodes[1] = temp
	addEdge(graph, nodes[0], nodes[1])

arr = []
DFS(arr, graph)
#print(graph)
#print(arr)
i = 0
while len(arr)-1 > i:
	printedArr = scc(graph, arr, i)
	print(printedArr)


	





