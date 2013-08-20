import sys
import locale
import time
import math
import random
import matplotlib.pyplot as plt

class City:
    def __init__( self , index = 0 , x = 0 , y = 0 ):
        self.index = index
        self.x = x
        self.y = y

    def __str__( self ):
        return 'x = %f, y = %f\n' % ( self.index , self.x , self.y )

    def __repr__( self ):
        return self.__str__()

def distance( city1 , city2 ):
    return( ( city1.x - city2.x ) ** 2 + ( city1.y - city2.y ) ** 2 ) ** 0.5

def totalDistance( cities ):
    dist = 0
    
    for i in range( len( cities ) - 1 ):
        dist += distance( cities[ i ] , cities[ i + 1 ] )

    if len( cities ) > 0:
        dist += distance( cities[ -1 ] , cities[ 0 ] )

    return dist

def plotCities( fid , cities ):
    fig = plt.figure( fid )
    ax = fig.add_subplot( 111 )

    citiesX = [ city.x for city in cities + [ cities[ 0 ] ] ]
    citiesY = [ city.y for city in cities + [ cities[ 0 ] ] ]

    ax.plot( citiesX , citiesY , 'gv--' )
    ax.grid()

    spacing = math.fabs( min( citiesX ) - max( citiesX ) ) * 0.1
    ax.set_xlim( min( citiesX ) - spacing , max( citiesX ) + spacing * 3 )
    ax.set_ylim( min( citiesY ) - spacing , max( citiesY ) + spacing )

    for index, city in enumerate( cities ):
        ax.text( city.x , city.y ,
                 '%d' % ( index + 1 ) ,
                 withdash = True ,
                 dashdirection = 1 ,
                 dashlength = 30 ,
                 rotation = 0 ,
                 dashrotation = 15 ,
                 dashpush = 10 )

    return ax

def makeRandomCities( ncity , x , y ):
    cities = []
    
    for i in range( ncity ):
        cities.append( City( i , x * random.random() , y * random.random() ) )

    return cities

def computeSwapIndices( index , n ):
    previousIndex = ( index - 1 + n ) % n
    nextIndex = ( index + 1 ) % n

    return ( previousIndex , nextIndex )

def swapDistance( cities , index1 , index2 ):
    index1 = min( index1 , index2 )
    index2 = max( index1 , index2 )

    ( previousIndex1 , nextIndex1 ) = computeSwapIndices( index1 , len( cities ) )
    ( previousIndex2 , nextIndex2 ) = computeSwapIndices( index2 , len( cities ) )

    distances = []
    distances.append( distance( cities[ previousIndex1 ] , cities[ index1 ] ) )
    distances.append( distance( cities[ index2 ] , cities[ nextIndex2 ] ) )

    if index1 == previousIndex2:
        distances.append( distance( cities[ index1 ] , cities[ index2 ] ) )
    else:
        distances.append( distance( cities[ index1 ] , cities[ nextIndex1 ] ) )
        distances.append( distance( cities[ previousIndex2 ] , cities[ index2 ] ) )

    return sum( distances )

def annealing( cities , startTemperature = 1.0e+300 , endTemperature = 0.1 , coolingFactor = 0.99 , iteration = 1 ):
    bestCities = cities[ : ]
    bestDistance = totalDistance( bestCities )

    currentDistances = []
    bestDistances = []
    iterations = []

    try:
        for i in range( iteration ):
            temperature = startTemperature

            currentCities = bestCities[ : ]
            currentDistance = bestDistance
            newDistance = bestDistance
            newCities = bestCities[ : ]

            step = 0
            while temperature > endTemperature:
                index = random.sample( range( len( newCities ) - 1 ) , 2 )
                index[ 0 ] += 1
                index[ 1 ] += 1

                beforeSwap = swapDistance( newCities , index[ 0 ] , index[ 1 ] )
                newCities[ index[ 0 ] ], newCities[ index[ 1 ] ] = newCities[ index[ 1 ] ], newCities[ index[ 0 ] ]
                afterSwap = swapDistance( newCities , index[ 0 ] , index[ 1 ] )

                newDistance = newDistance - beforeSwap + afterSwap

                diff = newDistance - currentDistance

                if diff < 0 or math.exp( -diff / temperature ) > random.random():
                    currentCities = newCities[ : ]
                    currentDistance = newDistance
                else:
                    newCities = currentCities[ : ]
                    newDistance = currentDistance

                if currentDistance < bestDistance:
                    bestCities = currentCities[ : ]
                    bestDistance = currentDistance

                if True:
                    currentDistances.append( currentDistance )
                    bestDistances.append( bestDistance )

                temperature = temperature * coolingFactor
                step = step + 1

                iterations.append( len( currentDistances ) )

    except KeyboardInterrupt, e:
        print 'Keyboard Interrupt.'

    return bestCities, bestDistances, currentDistances, iterations

def usage():
    print 'usage: %s [ ncity ] [i] [ cfactor ] [ stemp ] [ etemp ]\n' % sys.argv[ 0 ]
    print '   ncity: number of cities.'
    print '       i: number of iterations in the restart process.'
    print ' cfactor: cooling factor.'
    print '   stemp: initial temperature.'
    print '   etemp: temperature at which process will be stopped.'

if __name__ == '__main__':

    if len( sys.argv ) < 1:
        usage()
        sys.exit( 0 )
    
    locale.setlocale( locale.LC_ALL , '' )
    
    numberOfCities   =   int( sys.argv[ 1 ] )
    iteration        =   int( sys.argv[ 2 ] ) if len( sys.argv ) > 1 else 1
    coolingFactor    = float( sys.argv[ 3 ] ) if len( sys.argv ) > 3 else 0.95
    startTemperature = float( sys.argv[ 4 ] ) if len( sys.argv ) > 4 else 1.0e+10
    endTemperature   = float( sys.argv[ 5 ] ) if len( sys.argv ) > 5 else 0.1
    randomSeed       = -1

    if randomSeed == -1:
        random.seed()
    else:
        random.seed( randomSeed )

    startTime = time.time()
    cities = makeRandomCities( numberOfCities , 10000 , 10000 )

    citiesLength = len( cities ) if numberOfCities <= 0 else numberOfCities

    print 'Start simulated annealing...'

    cities = cities[ : numberOfCities ]
    ( newCities , bestDistances , currentDistances , iterations ) = annealing( cities , startTemperature , endTemperature , coolingFactor , iteration )
    endTime = time.time()

    beginDistance = totalDistance( cities )
    endDistance = totalDistance( newCities )

    possiblePath = locale.format( '%.0f' , math.factorial( numberOfCities ) , 1 )

    print '        Number of cities: ' , numberOfCities
    print 'Number of possible paths: ' , possiblePath
    print '             Improvement: %8.6f %%' % ( 100 * ( beginDistance - endDistance ) / beginDistance )
    print '                    Time: %8.6f seconds' % ( endTime - startTime )
    print '        Initial distance: %8.6f km' % beginDistance
    print '        Optimal distance: %8.6f km\n' % endDistance

    print 'Finish Simulated annealing.'

    cityMap = plotCities( 1 , cities )
    cityMap.set_title( 'Initial tour on %d cities\nDistance: %.6f' % ( len( cities ) , beginDistance ) )

    if iteration:
        cityMap = plotCities( 2 , newCities )
        cityMap.set_title( 'Optimal tour on %d cities\nDistance: %.6f on %d iteration(s)' % ( len( cities ) , endDistance , iteration ) )
        
    plt.show()
