from collections import defaultdict
from heapq import *

def primMST( nodeList , edgeList ):
    connectList = defaultdict( list )

    for node, node2, connect in edgeList:
        connectList[ node ].append( ( connect , node , node2 ) )
        connectList[ node2 ].append( ( connect , node2 , node ) )

    MST = []
    used = set( nodeList[ 0 ] )
    usableEdges = connectList[ nodeList[ 0 ] ][ : ]
    heapify( usableEdges )

    while usableEdges:
        cost, node, node2 = heappop( usableEdges )

        if node2 not in used:
            used.add( node2 )
            MST.append( ( node , node2 , cost ) )

            for edge in connectList[ node2 ]:
                if edge[ 2 ] not in used:
                    heappush( usableEdges , edge )

    return MST

Nodes = list( "ABCDEFG" )
Edges = [ ( "A" , "B" ,  7 ) , ( "A" , "D" , 5 ) ,
          ( "B" , "C" ,  8 ) , ( "B" , "D" , 9 ) , ( "B" , "E" , 7 ) ,
          ( "C" , "E" ,  5 ) ,
          ( "D" , "E" , 15 ) , ( "D" , "F" , 6 ) ,
          ( "E" , "F" ,  8 ) , ( "E" , "G" , 9 ) ,
          ( "F" , "G" , 11 ) ]

print ( "Prim's Algoritm: " )
print primMST( Nodes , Edges )
