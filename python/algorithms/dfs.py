'''
	description of file:
	implement the linear time strongly connected components 
	algorithm (depth first search algo).
    Given a directed graph G you first create the
	reverse graph Gr in linear time. Next, you run the 
	undirected connected components method, processing the 
	vertices in decreasing post order. You must make sure that 
	both steps can be carried out in O(|V|+|E|) time.
'''

from collections import defaultdict

#create container for nodes
#[[node, [reachable nodes]], [node, [reachable nodes]]]
#graph = []
graph = defaultdict(list)

'''
addEdge
function to add an edge to the graph list
'''
def addEdge(graph, u, v):
    graph[u].append(v)
 
'''
DFSRecursive
a helper function to DFS
computes the recursion through all connected vertices of a given vertex
to make sure all vertices are visited
'''
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
 		
'''
DFS
the depth first search algorithm.
we start from a root node and visit all possible nodes connected to this
root node, going down the tree, until we need to backtrack. We improve
efficiency by having a visited bool value so we don't traverse the same
node structure multiple times.
'''
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

'''
scc
creates an array of every child in the tree,
removing nodes as it traverses.
'''
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
#open the file to read the graph
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

#create the graph
arr = []
DFS(arr, graph)
#print(graph)
#print(arr)
i = 0
while len(arr)-1 > i:
	printedArr = scc(graph, arr, i)
	print(printedArr)


	





