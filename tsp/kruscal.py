from operator import itemgetter
from igraph import *

class DisjointSet( dict ) :
    def append( self , element ):
        self[ element ] = element

    def union( self , element , element2 ):
        self[ element2 ] = self[ element ]

    def find( self , element ):
        parent = self[ element ]

        while self[ parent ] != parent:
            parent = self[ parent ]

        self[ element ] = parent

        return parent;

def KruskalMST( nodeList , edgeList ):
    forest = DisjointSet()
    MST = []

    for i in nodeList:
        forest.append( i )

    size = len( nodeList ) - 1

    for edge in sorted( edgeList , key = itemgetter( 2 ) ):
        node, node2, _ = edge

        tree = forest.find( node )
        tree2 = forest.find( node2 )

        if tree != tree2:
            MST.append( edge )
            size -= 1

            if size == 0:
                return MST

            forest.union( tree , tree2 )

Nodes = list( "ABCDEFG" )
Edges = [ ( "A" , "B" ,  7 ) , ( "A" , "D" , 5 ) ,
          ( "B" , "C" ,  8 ) , ( "B" , "D" , 9 ) , ( "B" , "E" , 7 ) ,
          ( "C" , "E" ,  5 ) ,
          ( "D" , "E" , 15 ) , ( "D" , "F" , 6 ) ,
          ( "E" , "F" ,  8 ) , ( "E" , "G" , 9 ) ,
          ( "F" , "G" , 11 ) ]

g = Graph()
g.add_vertices( 7 )
g.add_edges( [ ( 0 , 1 ) , ( 0 , 3 ) ,
               ( 1 , 2 ) , ( 1 , 3 ) , ( 1 , 4 ) ,
               ( 2 , 4 ) ,
               ( 3 , 4 ) , ( 3 , 5 ) ,
               ( 4 , 5 ) , ( 4 , 6 ) ,
               ( 5 , 6 ) ] )

g.vs
g.vs[ "name" ] = [ "A" , "B" , "C" , "D" , "E" , "F", "G" ];
g.es[ "weight" ] = [ 7 , 5 , 8 , 9 , 7 , 5 , 15 , 6 , 8 , 9 , 11 ]
g.es[ "is_select" ] = [ True , True , False , False , True , True , False , True , False , True , False ]

visual_style = {}
visual_style[ "vertex_size" ] = 25
visual_style[ "vertex_color" ] = [ "white" ]
visual_style[ "vertex_label" ] = g.vs[ "name" ]
visual_style[ "edge_width" ] = [ 1 + 2 * int ( is_select ) for is_select in g.es[ "is_select" ] ]
edge_color = { 1 : 'green' , 0 : 'red' }
visual_style[ "edge_color" ] = [ edge_color[ int( is_select ) ] for is_select in g.es[ "is_select" ] ]
visual_style[ "layout" ] = g.layout( "fr" )
visual_style[ "bbox" ] = ( 400 , 400 )
visual_style[ "margin" ] = 20

plot( g , **visual_style )
